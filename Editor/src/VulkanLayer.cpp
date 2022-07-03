#include "VulkanLayer.h"

#include "Vulkan/VulkanContext.h"
#include "Vulkan/VulkanRenderer.h"
#include "Vulkan/VulkanTexture.h"
#include "Vulkan/VulkanUtil.h"
#include "Vulkan/VulkanMesh.h"
#include "Vulkan/VulkanInitializers.h"

#include "Core/Application.h"
#include "Core/Input.h"
#include "Core/Timer.h"
#include "Core/ResourceManager.h" // ASSETS dir
#include "Core/FileSystem.h"
#include "Core/Log.h"

#include "ImGui/VulkanImGuiLayer.h"

#include "Renderer/FrameGraph.h"
#include "Renderer/GlobalInfo.h"

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
static global::GlobalInfo globalInfo;
static FrameGraph frameGraph;

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
                50 * (i - maxLights * 0.5f),
                30,
                0
            );
        light.AddComponent<LightComponent>();
        light.GetComponent<LightComponent>().intensity = 100.0f;
    }

    editorCamera = CreateSharedPtr<Camera>(1600, 900, 0.1f, 1000.0f);
    editorCamera->nearPlane = 0.001f;
    editorCamera->farPlane = 10000.0f;
    cameraController.normalSpeed = 5;
    cameraController.SetActiveCamera(
        editorCamera,
        glm::vec3(0.0f, 0.0f, 1.0f), // position
        glm::vec3(0.0f, 0.0f, 0.0f) // euler angles
    );

    auto environment = scene->CreateEntity("environment");
    //environment.AddComponent<VulkanMeshComponent>(
    //    //ASSETS + "/Models/Sponza/gltf/Sponza.gltf"
    //    ASSETS + "/Models/helmet/scene.gltf"
    //);
    environment.GetComponent<TransformComponent>().scale = glm::vec3(0.3f);

    globalInfo.Init();

    // Post-process
    //// TODO replace with just one triangle for projection quad
    //float vertices[] =
    //{
    //    -1.0f,  1.0f, 0.0f,     0, 1,   0, 0, 0,
    //    -1.0f, -1.0f, 0.0f,     0, 0,   0, 0, 0,
    //     1.0f, -1.0f, 0.0f,     1, 0,   0, 0, 0,
    //     1.0f,  1.0f, 0.0f,     1, 1,   0, 0, 0,
    //};

    //uint32_t indices[] =
    //{
    //    0, 1, 2,
    //    2, 3, 0,
    //};

    //vertexBufferPtr = CreateSharedPtr<VulkanVertexBuffer>(vertices, sizeof(vertices));
    //indexBufferPtr = CreateSharedPtr<VulkanIndexBuffer>(indices, sizeof(indices) / sizeof(uint32_t));
    frameGraph.CreateRenderFrameGraph();

    VulkanContext::Get()->GetSwapChain()->CreateFramebuffers(frameGraph.GetRenderPass("opaque"));
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

    globalInfo.Update(editorCamera, scene, frameIdx);

    // TODO this should be called from the application loop
    vkImguiLayer->Begin();
    OnImGuiRender();
    vkImguiLayer->End();

    auto graphicsPipeline = VulkanContext::Get()->mFrameGraph->GetGraphicsPipeline("models");
    auto swapChainSize = VulkanContext::Get()->GetSwapChain()->GetImages().size();
    for (size_t frame = 0; frame < swapChainSize; ++frame)
    {
        auto commandBuffer = VulkanRenderer::BeginScene(frame);
        {
            std::vector<vk::DescriptorSet> descriptorSets{
                globalInfo.mDescriptorSets.at(0).get(),
            };

            commandBuffer.bindDescriptorSets(
                vk::PipelineBindPoint::eGraphics,
                graphicsPipeline->mPipelineLayout.get(),
                0,
                static_cast<uint32_t>(descriptorSets.size()), descriptorSets.data(),
                0, nullptr
            );

            commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, graphicsPipeline->mPipeline.get());

            auto group = scene->registry.group<VulkanMeshComponent>(entt::get<TransformComponent, TagComponent>);
            for (auto entity : group)
            {
                auto [transformComponent, meshComponent, tagComponent] = group.get<TransformComponent, VulkanMeshComponent, TagComponent>(entity);

                mPushConstBlock.model = transformComponent;
                commandBuffer.pushConstants(
                    graphicsPipeline->mPipelineLayout.get(),
                    vk::ShaderStageFlagBits::eVertex,
                    0, sizeof(VulkanGraphicsPipeline::TransformPushConstBlock),
                    &mPushConstBlock);

                // Draw mesh
                meshComponent.mesh->Draw(commandBuffer, frame);
            }

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

void VulkanLayer::ProcessUserInput()
{
    if (Input::GetMod(MOD_KEY_CONTROL) && Input::GetKeyDown(KEY_R))
    {
        ReconstructPipelines();
        //LOG_INFO("Finish reconstructing pipelines");
        CORE_INFO("Finish reconstructing pipelines");
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
    auto gpu = VulkanContext::Get()->GetDevice()->GetVulkanPhysicalDevice();
    auto props = gpu.getProperties();
    ImGui::Text(props.deviceName);
    std::string timerText(std::to_string(frameTime) + " ms/frame");
    ImGui::Text(timerText.c_str());

    ImGui::Separator();

    // TODO display pipelines
    // TODO display log

    ImGui::End();

    ImGui::Begin("Controls");

    ImGui::Separator();

    ImGui::Text("Camera");
    //ImGui::DragFloat3("Controller Position", (float*)&cameraController.position);
    //ImGui::DragFloat3("Camera Position", (float*)&editorCamera->pos);
    ImGui::DragFloat("Speed", &cameraController.normalSpeed);
    ImGui::DragFloat("Near", &editorCamera->nearPlane);
    ImGui::DragFloat("Far", &editorCamera->farPlane);

    ImGui::Separator();

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
    ctx->mFrameGraph->ReconstructFrameGraph();
}

bool VulkanLayer::OnWindowResize(WindowResizeEvent& e)
{
    if (e.GetWidth() == 0 || e.GetHeight() == 0) {
        return true;
    }

    editorCamera->width = e.GetWidth();
    editorCamera->height = e.GetHeight();
    editorCamera->SetProjection();

    VulkanContext::Get()->RecreateSwapChain();

    return false;
}
