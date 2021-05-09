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

//========================================================
struct CameraUBO
{
    glm::mat4 view;
    glm::mat4 proj;
    glm::mat4 viewProj;
};
static CameraUBO cameraUBO;

//========================================================
struct Light
{
    glm::vec4 pos;
    glm::vec4 color;
    glm::vec3 dir;
    float intensity;
};
const int maxNumLights = 100;
std::array<Light, maxNumLights> lights;

//========================================================
VulkanLayer::VulkanLayer()
    : Layer("VulkanLayer")
{
    mDevice = VulkanContext::Get()->GetDevice()->GetVulkanDevice();
}

void VulkanLayer::OnAttach()
{
    scene = CreateSharedPtr<Scene>();

    const int maxLights = 5;
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
        light.GetComponent<LightComponent>().intensity = 20.0f;
    }

    editorCamera = CreateSharedPtr<Camera>(1600, 900, 0.1f, 1000.0f);
    editorCamera->nearPlane = 0.001f;
    editorCamera->farPlane = 10000.0f;
    cameraController.normalSpeed = 15;
    cameraController.SetActiveCamera(
        editorCamera,
        glm::vec3(0.0f, 0.0f, 10.0f), // position
        glm::vec3(0.0f, 0.0f, 0.0f) // euler angles
    );

    auto environment = scene->CreateEntity("environment");
    environment.AddComponent<VulkanMeshComponent>(
        ASSETS + "/Models/Sponza/gltf/Sponza.gltf"
    );
    environment.GetComponent<TransformComponent>().scale = glm::vec3(0.1f);

    auto graphicsPipeline = VulkanContext::Get()->mFrameGraph.GetGraphicsPipeline("models");
    auto swapChainSize = VulkanContext::Get()->GetSwapChain()->GetImages().size();
    for (size_t i = 0; i < swapChainSize; ++i)
    {
        { // Create camera UBOs
            cameraUBOs.emplace_back(
                CreateSharedPtr<VulkanUniformBuffer>(
                    sizeof(CameraUBO)
                )
            );

            BufferLayout cameraLayout =
            {
                { ShaderDataType::MAT4, "view" },
                { ShaderDataType::MAT4, "proj" },
                { ShaderDataType::MAT4, "viewProj" },
            };

            cameraUBOs[i]->UpdateDescriptorSet(
                graphicsPipeline->mDescriptorSets[0][i].get(),
                cameraLayout, 0
            );
        }

        // TODO switch to storage buffer
        { // Create Light UBOs
            lightUBOs.emplace_back(
                CreateSharedPtr<VulkanUniformBuffer>(
                    sizeof(lights) + sizeof(glm::vec4)
                )
            );

            BufferLayout lightLayout =
            {
                { ShaderDataType::FLOAT4, "pos"       },
                { ShaderDataType::FLOAT4, "color"     },
                { ShaderDataType::FLOAT3, "dir"       },
                { ShaderDataType::FLOAT , "intensity" },
            };

            lightUBOs[i]->UpdateDescriptorSet(
                graphicsPipeline->mDescriptorSets[0][i].get(),
                1, maxNumLights*lightLayout.GetStride() + sizeof(glm::vec4)
            );
        }
    }
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
        light.pos = glm::vec4(transformComponent.position, 0.0f);
        light.dir = glm::vec3(0.0f);
        light.intensity = lightComponent.intensity;
        light.color = lightComponent.color;
        lights[numLights++] = light;
    }
    lightUBOs[frameIdx]->SetData(lights.data(), 0, sizeof(lights));
    lightUBOs[frameIdx]->SetData(&numLights, sizeof(lights), sizeof(uint32_t));

    VulkanImGuiLayer* vkImguiLayer = Application::Get().imguiLayer;

    // TODO this should be called from the application loop
    vkImguiLayer->Begin();
    OnImGuiRender();
    vkImguiLayer->End();

    auto graphicsPipeline = VulkanContext::Get()->mFrameGraph.GetGraphicsPipeline("models");

    for (size_t frame = 0; frame < 3; ++frame)
    {
        auto commandBuffer = VulkanRenderer::BeginScene(frame);
        {
            std::vector<vk::DescriptorSet> descriptorSets{
                graphicsPipeline->mDescriptorSets[0][frame].get(), // Camera, light ubo
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
    ImGui::End();

    ImGui::Begin("Controls");

    ImGui::Separator();

    ImGui::Text("Camera");
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

bool VulkanLayer::OnWindowResize(WindowResizeEvent& e)
{
    // TODO update dynamicStates of pipeline (viewport width, etc)
    editorCamera->width = e.GetWidth();
    editorCamera->height = e.GetHeight();
    editorCamera->SetProjection();

    VulkanContext::Get()->RecreateSwapChain();

    return false;
}
