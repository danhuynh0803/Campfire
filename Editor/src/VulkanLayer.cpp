#include "VulkanLayer.h"

#include "Vulkan/VulkanContext.h"
#include "Vulkan/VulkanRenderer.h"
#include "Vulkan/VulkanTexture.h"
#include "Vulkan/VulkanUtil.h"
#include "Vulkan/VulkanMesh.h"
#include "Vulkan/VulkanInitializers.h"
#include "Vulkan/VulkanPipeline.h"

#include "Core/Application.h"
#include "Core/Input.h"
#include "Core/Timer.h"
#include "Core/ResourceManager.h" // ASSETS dir
#include "Core/FileSystem.h"
#include "Core/Log.h"

#include "ImGui/VulkanImGuiLayer.h"

#include "Renderer/FrameGraph.h"
#include "Renderer/GlobalInfo.h"
#include "Vulkan/VulkanShader.h"

#include "Scene/Scene.h"
#include "Scene/Entity.h"
#include "Scene/Component.h"
#include "Scene/CameraController.h"

#include <glm/gtc/matrix_transform.hpp>

static SharedPtr<Camera> editorCamera;
static CameraController cameraController;
double frameTime = 0;
float metricTimer = 0.0;
static global::RenderContext globalRenderContext;
static FrameGraph frameGraph;
static SharedPtr<VulkanVertexBuffer> postProcessVbo;
static SharedPtr<VulkanIndexBuffer> postProcessIbo;
static SharedPtr<cf::Pipeline> deferredPipeline;
static SharedPtr<cf::Pipeline> modelPipeline;
static SharedPtr<cf::Pipeline> postProcessPipeline;
static vk::DescriptorImageInfo descriptorImageInfo;

namespace cf
{

// Encapsulate all needed objects for a renderpass
struct RenderPass
{
    std::vector<Attachment> attachments;
    uint32_t width, height;
    vk::UniqueFramebuffer framebuffer;
    vk::UniqueRenderPass renderPass;
    vk::DescriptorImageInfo descriptor;
    vk::UniquePipelineLayout layout;
};

};

cf::RenderPass offscreenPass;
cf::RenderPass postProcessPass;

vk::UniqueRenderPass CreatePostProcessPass()
{
    vk::AttachmentDescription colorAttachment;
    colorAttachment.format = VulkanContext::Get()->GetSwapChain()->GetFormat();
    colorAttachment.samples = vk::SampleCountFlagBits::e1;
    colorAttachment.loadOp = vk::AttachmentLoadOp::eDontCare;
    colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
    colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
    colorAttachment.initialLayout = vk::ImageLayout::eColorAttachmentOptimal;
    colorAttachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;

    vk::AttachmentReference colorAttachmentRef;
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;

    // Setup subpasses
    std::vector<vk::SubpassDescription> subpasses;
    vk::SubpassDescription subpass;
    subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = nullptr;
    subpasses.emplace_back(subpass);

    // Setup subpass dependencies
    std::vector<vk::SubpassDependency> subpassDependencies;
    vk::SubpassDependency dep;
    dep.srcSubpass = VK_SUBPASS_EXTERNAL;
    dep.dstSubpass = 0;
    dep.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests;
    dep.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests;
    dep.srcAccessMask = static_cast<vk::AccessFlagBits>(0);
    dep.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
    subpassDependencies.emplace_back(dep);

    // Setup render pass
    std::array<vk::AttachmentDescription, 1> attachments = { colorAttachment };
    vk::RenderPassCreateInfo renderPassCreateInfo {};
    renderPassCreateInfo.flags = vk::RenderPassCreateFlags();
    renderPassCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassCreateInfo.pAttachments = attachments.data();
    renderPassCreateInfo.subpassCount = static_cast<uint32_t>(subpasses.size());
    renderPassCreateInfo.pSubpasses = subpasses.data();
    renderPassCreateInfo.dependencyCount = static_cast<uint32_t>(subpassDependencies.size());
    renderPassCreateInfo.pDependencies = subpassDependencies.data();

    auto device = VulkanContext::Get()->GetDevice()->GetVulkanDevice();
    return device.createRenderPassUnique(renderPassCreateInfo);
}

SharedPtr<cf::Pipeline> CreatePostProcessPipeline()
{
    auto device = VulkanContext::Get()->GetDevice()->GetVulkanDevice();

    std::vector<std::vector<vk::DescriptorSetLayoutBinding>> descriptorSetLayoutBindings(1);
    { // Input ColorAttachment
        auto inputAttachment = vk::initializers::DescriptorSetLayoutBinding(
            vk::DescriptorType::eCombinedImageSampler,
            vk::ShaderStageFlagBits::eFragment,
            0);

        // Set 0
        descriptorSetLayoutBindings[0] = {
            inputAttachment,
        };
    }

    auto vs = CreateSharedPtr<VulkanShader>(SHADERS + "/deferred.vert");
    vk::PipelineShaderStageCreateInfo vsStageInfo{};
    vsStageInfo.stage  = vk::ShaderStageFlagBits::eVertex;
    vsStageInfo.module = vs->GetShaderModule();
    vsStageInfo.pName  = "main";

    auto fs = CreateSharedPtr<VulkanShader>(SHADERS + "/deferred.frag");
    vk::PipelineShaderStageCreateInfo fsStageInfo{};
    fsStageInfo.stage  = vk::ShaderStageFlagBits::eFragment;
    fsStageInfo.module = fs->GetShaderModule();
    fsStageInfo.pName  = "main";

    std::vector<vk::PipelineShaderStageCreateInfo> shaderStages = {
        vsStageInfo,
        fsStageInfo,
    };

    std::vector<cf::VertexComponent> components = {
        cf::VertexComponent::Position,
        cf::VertexComponent::UV,
    };

    return CreateSharedPtr<cf::Pipeline>(
        descriptorSetLayoutBindings,
        shaderStages,
        PipelineType::eGraphics,
        components,
        postProcessPass.renderPass.get()
    );
}

SharedPtr<cf::Pipeline> CreateDeferredPipeline()
{
    std::vector<std::vector<vk::DescriptorSetLayoutBinding>> descriptorSets(2);
    { // Environment descriptors
        // Camera
        auto camera = vk::initializers::DescriptorSetLayoutBinding(
            vk::DescriptorType::eUniformBuffer,
            vk::ShaderStageFlagBits::eAllGraphics,
            0);

        // Set 0
        descriptorSets[0] = {
            camera,
        };
    }

    // Textures to write out to MRTs
    { // Material descriptors
        // Albedo map
        auto albedo = vk::initializers::DescriptorSetLayoutBinding(
            vk::DescriptorType::eCombinedImageSampler,
            vk::ShaderStageFlagBits::eFragment,
            0);

        // Metallic map
        auto metallic = vk::initializers::DescriptorSetLayoutBinding(
            vk::DescriptorType::eCombinedImageSampler,
            vk::ShaderStageFlagBits::eFragment,
            1);

        // Normal map
        auto normal = vk::initializers::DescriptorSetLayoutBinding(
            vk::DescriptorType::eCombinedImageSampler,
            vk::ShaderStageFlagBits::eFragment,
            2);

        descriptorSets[1] = {
            albedo,
            metallic,
            normal,
        };
    }

    // Shaders
    auto vs = CreateSharedPtr<VulkanShader>(SHADERS + "/deferred.vert");
    vk::PipelineShaderStageCreateInfo vsStageInfo{};
    vsStageInfo.stage  = vk::ShaderStageFlagBits::eVertex;
    vsStageInfo.module = vs->GetShaderModule();
    vsStageInfo.pName  = "main";

    auto fs = CreateSharedPtr<VulkanShader>(SHADERS + "/deferred.frag");
    vk::PipelineShaderStageCreateInfo fsStageInfo{};
    fsStageInfo.stage  = vk::ShaderStageFlagBits::eFragment;
    fsStageInfo.module = fs->GetShaderModule();
    fsStageInfo.pName  = "main";

    std::vector<vk::PipelineShaderStageCreateInfo> shaderStages = {
        vsStageInfo,
        fsStageInfo,
    };

    std::vector<cf::VertexComponent> components = {
        cf::VertexComponent::Position,
        cf::VertexComponent::UV,
        cf::VertexComponent::Normal
    };

    // TODO crate deferred pass
    return CreateSharedPtr<cf::Pipeline>(
        descriptorSets,
        shaderStages,
        PipelineType::eGraphics,
        components,
        frameGraph.GetRenderPass("deferred")
    );
}

SharedPtr<cf::Pipeline> CreateModelPipeline()
{
    std::vector<std::vector<vk::DescriptorSetLayoutBinding>> descriptorSets(2);
    { // Environment descriptors
        // Camera
        auto camera = vk::initializers::DescriptorSetLayoutBinding(
            vk::DescriptorType::eUniformBuffer,
            vk::ShaderStageFlagBits::eAllGraphics,
            0);

        // TODO switch to storage buffer
        // Lights
        auto lights = vk::initializers::DescriptorSetLayoutBinding(
            vk::DescriptorType::eUniformBuffer,
            vk::ShaderStageFlagBits::eFragment,
            1);

        // Set 0
        descriptorSets[0] = {
            camera,
            lights,
        };
    }

    // TODO use spirv-cross to get descriptors layout
    { // Material descriptors
        // Albedo map
        auto albedo = vk::initializers::DescriptorSetLayoutBinding(
            vk::DescriptorType::eCombinedImageSampler,
            vk::ShaderStageFlagBits::eFragment,
            0);

        // Metallic map
        auto metallic = vk::initializers::DescriptorSetLayoutBinding(
            vk::DescriptorType::eCombinedImageSampler,
            vk::ShaderStageFlagBits::eFragment,
            1);

        // Normal map
        auto normal = vk::initializers::DescriptorSetLayoutBinding(
            vk::DescriptorType::eCombinedImageSampler,
            vk::ShaderStageFlagBits::eFragment,
            2);

        // Roughness map
        auto roughness = vk::initializers::DescriptorSetLayoutBinding(
            vk::DescriptorType::eCombinedImageSampler,
            vk::ShaderStageFlagBits::eFragment,
            3);

        // Ambient Occlusion map
        auto ambientOcclusion = vk::initializers::DescriptorSetLayoutBinding(
            vk::DescriptorType::eCombinedImageSampler,
            vk::ShaderStageFlagBits::eFragment,
            4);

        // Emissive map
        auto emissive = vk::initializers::DescriptorSetLayoutBinding(
            vk::DescriptorType::eCombinedImageSampler,
            vk::ShaderStageFlagBits::eFragment,
            5);

        // TextureMap usage
        auto mapUsage = vk::initializers::DescriptorSetLayoutBinding(
            vk::DescriptorType::eUniformBuffer,
            vk::ShaderStageFlagBits::eFragment,
            6);

        descriptorSets[1] = {
            albedo,
            metallic,
            normal,
            roughness,
            ambientOcclusion,
            emissive,
            mapUsage,
        };
    }

    // Shaders
    auto vs = CreateSharedPtr<VulkanShader>(SHADERS + "/model.vert");
    vk::PipelineShaderStageCreateInfo vsStageInfo{};
    vsStageInfo.stage  = vk::ShaderStageFlagBits::eVertex;
    vsStageInfo.module = vs->GetShaderModule();
    vsStageInfo.pName  = "main";

    auto fs = CreateSharedPtr<VulkanShader>(SHADERS + "/model.frag");
    vk::PipelineShaderStageCreateInfo fsStageInfo{};
    fsStageInfo.stage  = vk::ShaderStageFlagBits::eFragment;
    fsStageInfo.module = fs->GetShaderModule();
    fsStageInfo.pName  = "main";

    std::vector<vk::PipelineShaderStageCreateInfo> shaderStages = {
        vsStageInfo,
        fsStageInfo,
    };

    std::vector<cf::VertexComponent> components {
        cf::VertexComponent::Position,
        cf::VertexComponent::UV,
        cf::VertexComponent::Normal
    };

    return CreateSharedPtr<cf::Pipeline>(
        descriptorSets,
        shaderStages,
        PipelineType::eGraphics,
        components,
        frameGraph.GetRenderPass("forwardOpaque")
    );
}


//========================================================
VulkanLayer::VulkanLayer()
    : Layer("VulkanLayer")
{
    mDevice = VulkanContext::Get()->GetDevice()->GetVulkanDevice();
}

void VulkanLayer::OnAttach()
{
    scene = CreateSharedPtr<Scene>();

    const int maxLights = 1;
    for (int i = 0; i < maxLights; ++i)
    {
        auto light = scene->CreateEntity("light");
        light.GetComponent<TransformComponent>().position =
            glm::vec3(
                0.0f,
                7.0f,
                0.0f
            );
        light.GetComponent<TransformComponent>().scale =
            glm::vec3(
                2.0f,
                0.0f,
                0.0f
            );
        light.AddComponent<LightComponent>();
        light.GetComponent<LightComponent>().intensity = 5.0f;
    }

    editorCamera = CreateSharedPtr<Camera>(1600, 900, 0.1f, 1000.0f);
    editorCamera->nearPlane = 0.001f;
    editorCamera->farPlane = 10000.0f;
    cameraController.normalSpeed = 5;
    cameraController.SetActiveCamera(
        editorCamera,
        glm::vec3(0.0f, 0.0f, 5.0f), // position
        glm::vec3(0.0f, 0.0f, 0.0f) // euler angles
    );

    // Post-process
    //// TODO replace with just one triangle for projection quad
    float vertices[] =
    {
        -1.0f,  1.0f, 0.0f,     0, 1,   0, 0, 0,
        -1.0f, -1.0f, 0.0f,     0, 0,   0, 0, 0,
         1.0f, -1.0f, 0.0f,     1, 0,   0, 0, 0,
         1.0f,  1.0f, 0.0f,     1, 1,   0, 0, 0,
    };

    uint32_t indices[] =
    {
        0, 1, 2,
        2, 3, 0,
    };

    postProcessVbo = CreateSharedPtr<VulkanVertexBuffer>(vertices, sizeof(vertices));
    postProcessIbo = CreateSharedPtr<VulkanIndexBuffer>(indices, sizeof(indices) / sizeof(uint32_t));

    { // deferred opaque renderpass
        auto& opaquePass = frameGraph.AddRenderPass("opaque", RenderQueueFlagsBits::eGraphics);
        // TODO refactor attachment handling with subpass class
        { // deferred mrt subpass
            auto& desc = opaquePass.AddSubpass("mrt");
            desc.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
            desc.colorAttachmentCount = 1;
            //desc.pColorAttachments = &colorAttachmentRef;
            //desc.pDepthStencilAttachment = &depthAttachmentRef;
        }

        { // post process composition subpass
            auto& desc = opaquePass.AddSubpass("postprocess");
            desc.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
            desc.colorAttachmentCount = 1;
            //desc.pColorAttachments = &colorAttachmentRef;
            //desc.pDepthStencilAttachment = &depthAttachmentRef;
        }
    }
    frameGraph.Prepare();

    VulkanContext::Get()->GetSwapChain()->CreateFramebuffers(frameGraph.GetRenderPass("forwardOpaque"));

    modelPipeline = CreateModelPipeline();
    postProcessPass.renderPass = CreatePostProcessPass();
    postProcessPipeline = CreatePostProcessPipeline();
    // TODO setup postprocess as compute
    //postProcessPipeline = frameGraph.GetPipeline("postprocess");

    globalRenderContext.Init(modelPipeline);
    auto swapChain = VulkanContext::Get()->GetSwapChain();
    ResizeTexture(swapChain->GetWidth(), swapChain->GetHeight());

    // Scene Info
    auto environment = scene->CreateEntity("environment");
    environment.AddComponent<VulkanMeshComponent>(
        //ASSETS + "/Models/Sponza/gltf/Sponza.gltf"
        ASSETS + "/Models/helmet/scene.gltf"
    );
    // Minify sponza model scaling
    //environment.GetComponent<TransformComponent>().scale = glm::vec3(0.1f);
}

void VulkanLayer::ResizeTexture(uint32_t width, uint32_t height)
{
    // TODO for resize postprocess texture but this should be handled by rendergraph
    /*
    blankTexture = CreateSharedPtr<VulkanTexture2D>(width, height);

    descriptorImageInfo.sampler = blankTexture->GetSampler();
    descriptorImageInfo.imageView = blankTexture->GetImageView();
    descriptorImageInfo.imageLayout = vk::ImageLayout::eGeneral;


    // Write for post process pipeline read
    {
        auto postProcPipeline = frameGraph.GetPipeline("postprocess");
        vk::WriteDescriptorSet writeInfo{};
        // TODO wrap descriptorSet into some API
        // so it can be more easily understood that we're accessing set n
        writeInfo.dstSet = postProcPipeline->mDescriptorSets[0].get();
        writeInfo.dstBinding = 0;
        writeInfo.dstArrayElement = 0;
        writeInfo.descriptorCount = 1;
        writeInfo.descriptorType = vk::DescriptorType::eCombinedImageSampler;
        writeInfo.pImageInfo = &descriptorImageInfo;

        mDevice.updateDescriptorSets(1, &writeInfo, 0, nullptr);
    }
    */
}

void VulkanLayer::OnDetach()
{
}

void VulkanLayer::OnUpdate(float dt)
{
    ProcessUserInput();

    metricTimer -= dt;

    timer.Reset();

    // Current frame in flight
    auto frameIdx = VulkanContext::Get()->GetSwapChain()->GetCurrentImageIndex();

    // Update camera
    cameraController.OnUpdate(dt);

    VulkanImGuiLayer* vkImguiLayer = Application::Get().imguiLayer;

    globalRenderContext.Update(editorCamera, scene, frameIdx);

    // TODO this should be called from the application loop
    vkImguiLayer->Begin();
    OnImGuiRender();
    vkImguiLayer->End();

    auto swapChainSize = VulkanContext::Get()->GetSwapChain()->GetImages().size();

    // For now, all rendering will be at the same resolution
    // but this should reference the renderpass width/height
    vk::Extent2D extent {};
    extent.width = VulkanContext::Get()->GetSwapChain()->GetWidth();
    extent.height = VulkanContext::Get()->GetSwapChain()->GetHeight();

    vk::Rect2D renderArea {};
    renderArea.offset = vk::Offset2D {0, 0};
    renderArea.extent = extent;

    // Set dynamic viewport
    vk::Viewport viewport {};
    viewport.x = 0;
    viewport.y = 0;
    viewport.width  = extent.width;
    viewport.height = extent.height;
    viewport.minDepth = 0;
    viewport.maxDepth = 1;

    for (size_t frame = 0; frame < swapChainSize; ++frame)
    {
        auto& commandBuffer = VulkanContext::Get()->GetSwapChain()->GetCommandBuffer(frame);

        vk::CommandBufferBeginInfo beginInfo {};
        beginInfo.flags = vk::CommandBufferUsageFlagBits::eSimultaneousUse;
        beginInfo.pInheritanceInfo = nullptr;
        commandBuffer.begin(beginInfo);

        commandBuffer.setScissor(0, 1, &renderArea);
        commandBuffer.setViewport(0, 1, &viewport);

        // Clear value
        std::array<vk::ClearValue, 2> clearValues;
        // Off-screen depth pass
        {
            clearValues[0].color = vk::ClearColorValue(
                std::array<float, 4>({ 0.0f, 0.0f, 0.0f, 1.0f })
            );
            clearValues[1].depthStencil = vk::ClearDepthStencilValue{ 1.0f, 0 };
            vk::RenderPassBeginInfo renderPassBeginInfo {};
            //renderPassBeginInfo.renderPass = renderPass;
            //renderPassBeginInfo.framebuffer = framebuffer;
            //renderPassBeginInfo.renderArea = renderArea;
            //renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
            //renderPassBeginInfo.pClearValues = clearValues.data();

            //commandBuffer.beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);
            //commandBuffer.endRenderPass();
        }

        // On-screen pass
        //auto commandBuffer = VulkanRenderer::BeginScene(frame, frameGraph.GetRenderPass("opaque"));
        {
            clearValues[0].color = vk::ClearColorValue(
                std::array<float, 4>({ 0.2f, 0.3f, 0.3f, 1.0f })
            );
            clearValues[1].depthStencil = vk::ClearDepthStencilValue{ 1.0f, 0 };

            auto framebuffer = VulkanContext::Get()->GetSwapChain()->GetFramebuffer(frame);

            vk::RenderPassBeginInfo renderPassBeginInfo {};
            renderPassBeginInfo.renderPass = frameGraph.GetRenderPass("forwardOpaque");;
            renderPassBeginInfo.framebuffer = framebuffer;
            renderPassBeginInfo.renderArea = renderArea;
            renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
            renderPassBeginInfo.pClearValues = clearValues.data();

            commandBuffer.beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);

            // TODO read from offscreen passes
            //commandBuffer.bindDescriptorSets(
            //    vk::PipelineBindPoint::eGraphics,
            //    postProcessPipeline->mPipelineLayout.get(),
            //    0, // set
            //    1, // size
            //    &postProcessPipeline->mDescriptorSets[0].get(),
            //    0,
            //    nullptr
            //);
            //commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, postProcessPipeline->mPipeline.get());

            std::vector<vk::DescriptorSet> descriptorSets{
                globalRenderContext.mDescriptorSets.at(0).get(),
            };

            commandBuffer.bindDescriptorSets(
                vk::PipelineBindPoint::eGraphics,
                modelPipeline->mPipelineLayout.get(),
                0,
                static_cast<uint32_t>(descriptorSets.size()), descriptorSets.data(),
                0, nullptr
            );
            commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, modelPipeline->mPipeline.get());

            // TODO render off screen
            auto group = scene->registry.group<VulkanMeshComponent>(entt::get<TransformComponent, TagComponent>);
            for (auto entity : group)
            {
                auto [transformComponent, meshComponent, tagComponent] = group.get<TransformComponent, VulkanMeshComponent, TagComponent>(entity);

                mPushConstBlock.model = transformComponent;
                commandBuffer.pushConstants(
                    modelPipeline->mPipelineLayout.get(),
                    vk::ShaderStageFlagBits::eVertex,
                    0, sizeof(VulkanGraphicsPipeline::TransformPushConstBlock),
                    &mPushConstBlock);

                // Draw mesh
                meshComponent.mesh->Draw(commandBuffer, modelPipeline->mPipelineLayout.get());
            }

            vkImguiLayer->mImGuiImpl->DrawFrame(commandBuffer);

            commandBuffer.endRenderPass();
        }

        // Post-process
        {
            // Post process quad
            //VulkanRenderer::DrawIndexed(
            //    commandBuffer,
            //    postProcessVbo->GetBuffer(),
            //    postProcessIbo->GetBuffer(),
            //    postProcessIbo->GetCount()
            //);

            //vkImguiLayer->mImGuiImpl->DrawFrame(commandBuffer);
        }

        //VulkanRenderer::EndScene(commandBuffer);
        commandBuffer.end();
    }


    if (metricTimer <= 0.0)
    {
        frameTime = timer.GetTime();
        metricTimer = 1.0f;
    }
}

void VulkanLayer::ProcessUserInput()
{
    if (Input::GetMod(MOD_KEY_CONTROL) && Input::GetKeyDown(KEY_R))
    {
        // TODO make a menu in imgui to reconstruct certain pipelines and not all of them
        ReconstructPipelines();
        // TODO enable console widget for LOG
        //LOG_INFO("Finish reconstructing pipelines");
        CORE_INFO("Finish reconstructing pipelines");
    }

    if (Input::GetMouseButton(MOUSE_BUTTON_RIGHT))
    {
        // Actions
    }

    // Reset camera position
    if (Input::GetKeyDown(KEY_F))
    {
        cameraController.ResetActiveCamera(
            editorCamera,
            glm::vec3(0.0f, 0.0f, 30.0f), // position
            glm::vec3(0.0f, 0.0f, 0.0f)   // euler angles
        );
    }
}

void VulkanLayer::OnImGuiRender()
{
    static bool showHierarchy = true;
    static bool showInspector = true;
    mHierarchyWidget.ShowHierarchy(scene, editorCamera, &showHierarchy);
    auto & selectedEntity = mHierarchyWidget.GetSelectedEntity();
    if (selectedEntity)
    {
        mInspectorWidget.ShowInspector(selectedEntity, &showInspector);

        //if (/*allowViewportCameraEvents &&*/ !Input::GetMouseButtonDown(MOUSE_BUTTON_RIGHT))
        //{
        //    if (Input::GetKeyDown(KEY_W))
        //        mTransformWidget.operation = ImGuizmo::TRANSLATE;
        //    if (Input::GetKeyDown(KEY_E))
        //        mTransformWidget.operation = ImGuizmo::ROTATE;
        //    if (Input::GetKeyDown(KEY_R))
        //        mTransformWidget.operation = ImGuizmo::SCALE;
        //}
        //mTransformWidget.EditTransform(selectedEntity, *editorCamera);
    }

    ImGui::Begin("Metrics");
    {
        auto gpu = VulkanContext::Get()->GetDevice()->GetVulkanPhysicalDevice();
        auto props = gpu.getProperties();
        ImGui::Text(props.deviceName);
        std::string timerText(std::to_string(frameTime) + " ms/frame");
        ImGui::Text(timerText.c_str());
        ImGui::Separator();
    }
    ImGui::End();

    ImGui::Begin("Controls");
    {
        ImGui::Separator();

        ImGui::Text("Camera");
        //ImGui::DragFloat3("Controller Position", (float*)&cameraController.position);
        //ImGui::DragFloat3("Camera Position", (float*)&editorCamera->pos);
        ImGui::DragFloat("Speed", &cameraController.normalSpeed);
        ImGui::DragFloat("Near", &editorCamera->nearPlane);
        ImGui::DragFloat("Far", &editorCamera->farPlane);

        ImGui::Separator();

        ImGui::Text("Camera Model Matrix");
        glm::mat4 invView = glm::inverse(editorCamera->GetViewMatrix());
        ImGui::DragFloat4("", (float*)&invView[0], 0.01f);
        ImGui::DragFloat4("", (float*)&invView[1], 0.01f);
        ImGui::DragFloat4("", (float*)&invView[2], 0.01f);
        ImGui::DragFloat4("", (float*)&invView[3], 0.01f);

        ImGui::Separator();
    }
    ImGui::End();
}

void VulkanLayer::OnEvent(Event& event)
{
    cameraController.OnEvent(event);
    EventDispatcher dispatcher(event);
    dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(VulkanLayer::OnWindowResize));
}

void VulkanLayer::ReconstructPipelines()
{
    auto ctx = VulkanContext::Get();
    ctx->GetDevice()->GetVulkanDevice().waitIdle();

    modelPipeline = CreateModelPipeline();
}

bool VulkanLayer::OnWindowResize(WindowResizeEvent& e)
{
    if (e.GetWidth() == 0 || e.GetHeight() == 0) {
        return true;
    }

    editorCamera->width = e.GetWidth();
    editorCamera->height = e.GetHeight();
    editorCamera->SetProjection();

    VulkanContext::Get()->RecreateSwapChain(frameGraph.GetRenderPass("opaque"));
    ResizeTexture(e.GetWidth(), e.GetHeight());

    return false;
}
