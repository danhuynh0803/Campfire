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

struct TransformUBO
{
    glm::mat4 model;
};
static TransformUBO transformUBO;

struct LightUBO
{
    glm::vec4 pos = glm::vec4(30, 30, 2.5f, 0);
    glm::vec4 color = glm::vec4(1, 1, 1, 1);
    glm::vec4 dir = glm::vec4(1, -1, 0, 0);
};
static LightUBO lightUBO;

VulkanLayer::VulkanLayer()
    : Layer("VulkanLayer")
{
}

Entity e;
Entity f;

void VulkanLayer::OnAttach()
{
    // initial scene
    scene = CreateSharedPtr<Scene>();
    e = scene->CreateEntity("Environment");
    e.GetComponent<TransformComponent>().position = glm::vec3(2.0f, 0.0f, 0.0f);
    //env.GetComponent<TransformComponent>().scale = glm::vec3(0.1f, 0.1f, 0.1f);
    e.AddComponent<VulkanMeshComponent>(
        //ASSETS + "/Models/primitives/sphere.fbx"
        //ASSETS + "/Models/ganon/scene.gltf"
        ASSETS + "/Models/cyborg/cyborg.obj"
        //ASSETS + "/Models/nanosuit/nanosuit.obj"
        //ASSETS + "/Models/helmet/scene.gltf"
        //ASSETS + "/Models/Sponza/glTF/Sponza.gltf"
    );

    f = scene->CreateEntity("helmet");
    f.GetComponent<TransformComponent>().position = glm::vec3(0.0f, 0.0f, 0.0f);
    //e.GetComponent<TransformComponent>().scale = glm::vec3(1.0f, 1.0f, 1.0f);
    f.AddComponent<VulkanMeshComponent>(
        //ASSETS + "/Models/primitives/sphere.fbx"
        //ASSETS + "/Models/ganon/scene.gltf"
        //ASSETS + "/Models/cyborg/cyborg.obj"
        //ASSETS + "/Models/nanosuit/nanosuit.obj"
        //ASSETS + "/Models/helmet/scene.gltf"
        ASSETS + "/Models/helmet/scene.gltf"
    );

    editorCamera = CreateSharedPtr<Camera>(1600, 900, 0.1f, 1000.0f);
    editorCamera->nearPlane = 0.001f;
    editorCamera->farPlane = 10000.0f;
    cameraController.normalSpeed = 15;
    cameraController.SetActiveCamera(
        editorCamera,
        glm::vec3(0.0f, 0.0f, 10.0f), // position
        glm::vec3(0.0f, 0.0f, 0.0f) // euler angles
    );

    auto& environmentSets = VulkanContext::Get()->GetPipeline()->uniformDescriptorSets;
    auto& transformSets = VulkanContext::Get()->GetPipeline()->transformDescriptorSets;

    // TODO match with swapchainImages size
    for (size_t i = 0; i < 3; ++i)
    {
        { // Create camera UBOs
            cameraUBOs.emplace_back(
                CreateSharedPtr<VulkanUniformBuffer>(
                    sizeof(CameraUBO),
                    environmentSets[i].get()
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
                    sizeof(LightUBO),
                    environmentSets[i].get()
                )
            );
            BufferLayout lightLayout =
            {
                { ShaderDataType::FLOAT4, "pos" },
                { ShaderDataType::FLOAT4, "color" },
                { ShaderDataType::FLOAT4, "dir" },
            };
            lightUBOs[i]->SetLayout(lightLayout, 1);
        }

        { // Create Transform UBOs
            transformUBOs.emplace_back(
                CreateSharedPtr<VulkanUniformBuffer>(
                    sizeof(TransformUBO),
                    transformSets[i].get()
                )
            );
            BufferLayout transformLayout =
            {
                { ShaderDataType::MAT4, "model" },
            };
            transformUBOs[i]->SetLayout(transformLayout, 0);
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

    // Update transform ubo
    //static float rotation = 0;
    //rotation += 180 * dt;
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(0.1f));
    //model = glm::rotate(model, glm::radians(rotation), glm::vec3(0, 0, 1));
    //transformUBO.model = model;
    //transformUBOs[frameIdx]->SetData(&transformUBO, 0, sizeof(TransformUBO));

    // Update camera ubo
    cameraUBO.view = editorCamera->GetViewMatrix();
    cameraUBO.proj = editorCamera->GetProjMatrix();
    cameraUBO.proj[1][1] *= -1;
    cameraUBO.viewProj = cameraUBO.proj * cameraUBO.view;
    cameraUBOs[frameIdx]->SetData(&cameraUBO, 0, sizeof(CameraUBO));

    // Update light ubo
    lightUBOs[frameIdx]->SetData(&lightUBO, 0, sizeof(LightUBO));

    VulkanImGuiLayer* vkImguiLayer = Application::Get().imguiLayer;

    vkImguiLayer->Begin();
    OnImGuiRender();
    vkImguiLayer->End();

    for (size_t i = 0; i < 3; ++i)
    {
        // Render scene and imgui
        auto commandBuffer = VulkanRenderer::BeginScene(i);
            auto group = scene->registry.group<VulkanMeshComponent>(entt::get<TransformComponent>);
            for (auto entity : group)
            {
                auto [transformComponent, meshComponent] = group.get<TransformComponent, VulkanMeshComponent>(entity);

                transformUBO.model = transformComponent;
                transformUBOs[i]->SetData(&transformUBO, 0, sizeof(TransformUBO));
                auto& transformDescriptors = VulkanContext::Get()->GetPipeline()->transformDescriptorSets;
                // Update transforms
                commandBuffer.bindDescriptorSets(
                    vk::PipelineBindPoint::eGraphics,
                    VulkanContext::Get()->GetPipeline()->GetVulkanPipelineLayout(),
                    2,
                    1, &transformUBOs[i]->mDescriptorSet,
                    0, nullptr
                );

                //// Draw mesh
                SharedPtr<vk::VulkanMesh> mesh = meshComponent;
                mesh->Draw(commandBuffer);
            }

            vkImguiLayer->mImGuiImpl->DrawFrame(commandBuffer);
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
    ImGui::Begin("Metrics");
    auto gpu = VulkanContext::Get()->GetDevice()->GetVulkanPhysicalDevice();
    auto props = gpu.getProperties();
    ImGui::Text(props.deviceName);
    std::string timerText(std::to_string(frameTime) + " ms/frame");
    ImGui::Text(timerText.c_str());
    ImGui::End();

    ImGui::Begin("Controls");

    ImGui::Separator();

    if (e.HasComponent<TransformComponent>())
    {
        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        if (ImGui::TreeNode("Transform"))
        {
            auto& transform = e.GetComponent<TransformComponent>();

            ImGui::DragFloat3("Position", (float*)&transform.position, 0.01f);
            ImGui::DragFloat3("Rotation", (float*)&transform.euler, 0.01f);
            ImGui::DragFloat3("Scale", (float*)&transform.scale, 0.01f);

            ImGui::TreePop();
        }
        ImGui::Separator();

    }

    if (e.HasComponent<VulkanMeshComponent>())
    {
        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        if (ImGui::TreeNode("Mesh"))
        {
            if (ImGui::Button("Load Mesh"))
            {
                SharedPtr<vk::VulkanMesh> mesh = e.GetComponent<VulkanMeshComponent>();
                std::string path = FileSystem::OpenFile();
                if (path.compare("") != 0) // No file selected
                {
                    mesh.reset(new vk::VulkanMesh(path));
                }
            }

            ImGui::TreePop();
        }
    }

    ImGui::Separator();

    ImGui::Text("Camera");
    ImGui::DragFloat("Speed", &cameraController.normalSpeed);
    ImGui::DragFloat("Near", &editorCamera->nearPlane);
    ImGui::DragFloat("Far", &editorCamera->farPlane);

    ImGui::Separator();

    // Light Controls
    ImGui::DragFloat4("Light Pos", (float*)&lightUBO.pos, 0.01f);
    ImGui::ColorEdit4("Light Color", (float*)&lightUBO.color, 0.01f);
    ImGui::DragFloat4("Light Dir", (float*)&lightUBO.dir, 0.01f);

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
    editorCamera->width = e.GetWidth();
    editorCamera->height = e.GetHeight();
    editorCamera->SetProjection();

    VulkanContext::Get()->RecreateSwapChain();

    return false;
}
