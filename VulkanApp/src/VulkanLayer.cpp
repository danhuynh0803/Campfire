#include "VulkanLayer.h"

#include "Vulkan/VulkanContext.h"
#include "Vulkan/VulkanRenderer.h"
#include "Vulkan/VulkanTexture.h"
#include "Vulkan/VulkanUtil.h"
#include "Vulkan/VulkanMesh.h"

#include "Core/Input.h"
#include "Core/Timer.h"
#include "Core/ResourceManager.h" // ASSETS dir
#include "Core/FileSystem.h"

#include "Scene/Scene.h"
#include "Scene/Entity.h"
#include "Scene/Component.h"
#include "Scene/CameraController.h"

#include <glm/gtc/matrix_transform.hpp>

#include <array>

static SharedPtr<Camera> editorCamera;
static CameraController cameraController;
double frameTime = 0;
float metricTimer = 0.0;

struct CameraUBO
{
    glm::mat4 view;
    glm::mat4 proj;
    glm::mat4 viewProj;
};
static CameraUBO cameraUBO;

struct Light
{
    glm::vec4 pos;
    glm::vec4 color;
    glm::vec3 dir;
    float intensity;
};
const int maxNumLights = 100;
std::array<Light, maxNumLights> lights;

VulkanLayer::VulkanLayer()
    : Layer("VulkanLayer")
{
    mDevice = VulkanContext::Get()->GetDevice()->GetVulkanDevice();
}

void VulkanLayer::OnAttach()
{
    scene = CreateSharedPtr<Scene>();

    editorCamera = CreateSharedPtr<Camera>(1600, 900, 0.1f, 1000.0f);
    editorCamera->nearPlane = 0.001f;
    editorCamera->farPlane = 10000.0f;
    cameraController.normalSpeed = 15;
    cameraController.SetActiveCamera(
        editorCamera,
        glm::vec3(0.0f, 0.0f, 10.0f), // position
        glm::vec3(0.0f, 0.0f, 0.0f) // euler angles
    );

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

    vertexBufferPtr = CreateSharedPtr<VulkanVertexBuffer>(vertices, sizeof(vertices));
    indexBufferPtr = CreateSharedPtr<VulkanIndexBuffer>(indices, sizeof(indices) / sizeof(uint32_t));

    // TODO match with swapchainImages size
    for (size_t i = 0; i < 3; ++i)
    {
        { // Create camera UBOs
            cameraUBOs.emplace_back(
                CreateSharedPtr<VulkanUniformBuffer>(
                    sizeof(CameraUBO),
                    VulkanContext::Get()->GetGraphicsPipeline()->mDescriptorSets[0][i].get()
                )
            );
            BufferLayout cameraLayout =
            {
                { ShaderDataType::MAT4, "view" },
                { ShaderDataType::MAT4, "proj" },
                { ShaderDataType::MAT4, "viewProj" },
            };
            cameraUBOs[i]->SetLayout(cameraLayout, 0);
        }

        { // Create Light UBOs
            lightUBOs.emplace_back(
                CreateSharedPtr<VulkanUniformBuffer>(
                    sizeof(lights) + sizeof(glm::vec4),
                    VulkanContext::Get()->GetGraphicsPipeline()->mDescriptorSets[0][i].get()
                )
            );
            BufferLayout lightLayout =
            {
                { ShaderDataType::FLOAT4, "pos" },
                { ShaderDataType::FLOAT4, "color" },
                { ShaderDataType::FLOAT3, "dir" },
                { ShaderDataType::FLOAT, "intensity" },
            };
            lightUBOs[i]->SetLayout(1, maxNumLights*lightLayout.GetStride() + sizeof(glm::vec4));
        }
    }

    auto graphicsPipeline = VulkanContext::Get()->GetGraphicsPipeline();
    auto computePipeline = VulkanContext::Get()->mComputePipeline;

    // Update post compute graphics descriptorset that reads in the processed image
    vk::WriteDescriptorSet writeInfo {};
    writeInfo.dstSet = graphicsPipeline->mDescriptorSets[1][0].get();
    writeInfo.dstBinding = 0;
    writeInfo.dstArrayElement = 0;
    writeInfo.descriptorCount = 1;
    writeInfo.descriptorType = vk::DescriptorType::eCombinedImageSampler;
    writeInfo.pImageInfo = &computePipeline->mDescriptorImageInfo;

    mDevice.updateDescriptorSets(1, &writeInfo, 0, nullptr);

    vk::FenceCreateInfo fenceInfo;
    fenceInfo.flags = vk::FenceCreateFlagBits::eSignaled;
    computeFence = mDevice.createFenceUnique(fenceInfo);
}

void VulkanLayer::OnDetach()
{
}

void VulkanLayer::OnUpdate(float dt)
{
    metricTimer -= dt;

    timer.Reset();

    // Current frame in flight
    auto frameIdx = VulkanContext::Get()->GetSwapChain()->GetCurrentImageIndex();

    // Update camera
    cameraController.OnUpdate(dt);

    // Update camera ubo
    cameraUBO.view = editorCamera->GetViewMatrix();
    cameraUBO.proj = editorCamera->GetProjMatrix();
    cameraUBO.proj[1][1] *= -1;
    cameraUBO.viewProj = cameraUBO.proj * cameraUBO.view;
    cameraUBOs[frameIdx]->SetData(&cameraUBO, 0, sizeof(CameraUBO));

    // Update light ubo
    uint32_t numLights = 0;
    auto group = scene->registry.group<LightComponent>(entt::get<TransformComponent>);
    for (auto entity : group)
    {
        auto [transformComponent, lightComponent] = group.get<TransformComponent, LightComponent>(entity);

        Light light;
        light.pos       = glm::vec4(transformComponent.position, 0.0f);
        light.dir       = glm::vec3(0.0f);
        light.intensity = lightComponent.intensity;
        light.color     = lightComponent.color;
        lights[numLights++] = light;
    }
    lightUBOs[frameIdx]->SetData(lights.data(), 0, sizeof(lights));
    lightUBOs[frameIdx]->SetData(&numLights, sizeof(lights), sizeof(uint32_t));

    VulkanImGuiLayer* vkImguiLayer = Application::Get().imguiLayer;

    // TODO this should be called from the application loop
    vkImguiLayer->Begin();
    OnImGuiRender();
    vkImguiLayer->End();

    // Submit compute command
    vk::SubmitInfo computeSubmitInfo {};
    computeSubmitInfo.commandBufferCount = 1;
    computeSubmitInfo.pCommandBuffers = &VulkanContext::Get()->mComputePipeline->mCmdBuffers.at(0).get();

    mDevice.waitForFences(computeFence.get(), VK_TRUE, UINT64_MAX);
    mDevice.resetFences(computeFence.get());
    auto computeQueue = VulkanContext::Get()->GetDevice()->GetQueue(QueueFamilyType::COMPUTE);
    computeQueue.submit(computeSubmitInfo, computeFence.get());

    // TODO add image barrier from compute to fragment

    for (size_t frame = 0; frame < 3; ++frame)
    {
        auto commandBuffer = VulkanRenderer::BeginScene(frame);
        {
            //auto group = scene->registry.group<VulkanMeshComponent>(entt::get<TransformComponent, TagComponent>);
            //for (auto entity : group)
            //{
            //    auto [transformComponent, meshComponent, tagComponent] = group.get<TransformComponent, VulkanMeshComponent, TagComponent>(entity);

            //    mPushConstBlock.model = transformComponent;
            //    commandBuffer.pushConstants(
            //        VulkanContext::Get()->GetGraphicsPipeline()->mPipelineLayout.get(),
            //        vk::ShaderStageFlagBits::eVertex,
            //        0, sizeof(VulkanGraphicsPipeline::TransformPushConstBlock),
            //        &mPushConstBlock);

            //    // Draw mesh
            //    meshComponent.mesh->Draw(commandBuffer, frame);
            //}

            mPushConstBlock.model = glm::mat4(1.0f);
            commandBuffer.pushConstants(
                VulkanContext::Get()->GetGraphicsPipeline()->mPipelineLayout.get(),
                vk::ShaderStageFlagBits::eVertex,
                0, sizeof(VulkanGraphicsPipeline::TransformPushConstBlock),
                &mPushConstBlock
            );

            commandBuffer.bindDescriptorSets(
                vk::PipelineBindPoint::eGraphics,
                VulkanContext::Get()->GetGraphicsPipeline()->mPipelineLayout.get(),
                1,
                1,
                &VulkanContext::Get()->GetGraphicsPipeline()->mDescriptorSets[1][0].get(),
                0,
                nullptr
            );

            VulkanRenderer::DrawIndexed(commandBuffer, vertexBufferPtr->GetBuffer(), indexBufferPtr->GetBuffer(), indexBufferPtr->GetCount());

            vkImguiLayer->mImGuiImpl->DrawFrame(commandBuffer);
        }
        VulkanRenderer::EndScene(commandBuffer);
    }

    if (metricTimer <= 0.0)
    {
        frameTime = timer.GetTime();
        metricTimer = 1.0f;
    }
}

void VulkanLayer::OnImGuiRender()
{
    SceneHierarchy(scene);

    ImGui::Begin("Metrics");
    auto gpu = VulkanContext::Get()->GetDevice()->GetVulkanPhysicalDevice();
    auto props = gpu.getProperties();
    ImGui::Text(props.deviceName);
    std::string timerText(std::to_string(frameTime) + " ms/frame");
    ImGui::Text(timerText.c_str());
    ImGui::End();

    ImGui::Begin("Controls");

    ImGui::Separator();

    ImGui::Text("Camera");
    ImGui::DragFloat("Speed", &cameraController.normalSpeed);
    ImGui::DragFloat("Near", &editorCamera->nearPlane);
    ImGui::DragFloat("Far", &editorCamera->farPlane);

    ImGui::Separator();

    // Light Controls
    //ImGui::DragFloat4("Light Pos", (float*)&lightUBO.pos, 0.01f);
    //ImGui::ColorEdit4("Light Color", (float*)&lightUBO.color, 0.01f);
    //ImGui::DragFloat3("Light Dir", (float*)&lightUBO.dir, 0.01f);
    //ImGui::DragFloat("Light Intensity", (float*)&lightUBO.intensity, 0.01f);

    ImGui::End();
}

void VulkanLayer::SceneHierarchy(SharedPtr<Scene> scene)
{
    ImGui::Begin("Scene Hierarchy");

    int rootIdx = 0;
    static int selected = -1;
    for (auto entityPair : scene->GetEntityMap())
    {
        Entity entity = entityPair.second;
        std::string tag = entity.GetComponent<TagComponent>().tag;
        if (ImGui::Selectable(tag.c_str(), selected == rootIdx))
        {
            selected = rootIdx;
        }
        ++rootIdx;
    }

    ImGui::End();
}

void VulkanLayer::OnEvent(Event& event)
{
    cameraController.OnEvent(event);

    EventDispatcher dispatcher(event);
    dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(VulkanLayer::OnWindowResize));
}

bool VulkanLayer::OnWindowResize(WindowResizeEvent& e)
{
    // TODO update dynamicStates of pipeline (viewport width, etc)
    editorCamera->width = e.GetWidth();
    editorCamera->height = e.GetHeight();
    editorCamera->SetProjection();

    VulkanContext::Get()->RecreateSwapChain();

    return false;
}
