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
const int numLights = 100;

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
std::array<Light, numLights> lights;

VulkanLayer::VulkanLayer()
    : Layer("VulkanLayer")
{
}

Entity e;

void VulkanLayer::OnAttach()
{
    // initial scene
    scene = CreateSharedPtr<Scene>();
    e = scene->CreateEntity("Environment");
    e.GetComponent<TransformComponent>().position = glm::vec3(2.0f, 0.0f, -15.0f);
    e.GetComponent<TransformComponent>().scale = glm::vec3(.1f, .1f, .1f);
    e.AddComponent<VulkanMeshComponent>(
        ASSETS + "/Models/Sponza/glTF/Sponza.gltf"
    );

    int maxRow = 10;
    int maxCol = 10;
    int maxWidth = 300;
    int maxLength = 300;
    for (int i = 0; i < maxRow; ++i)
    for (int j = 0; j < maxCol; ++j)
    {
        auto l = scene->CreateEntity("light" + std::to_string(i));
        glm::vec3 position(
            -maxWidth/2.0f + static_cast<float>(maxWidth)/maxCol * j,
             2.0f,
             maxLength/2.0f - static_cast<float>(maxLength)/maxRow * i
        );
        l.GetComponent<TransformComponent>().position = position;
        l.AddComponent<LightComponent>();
        l.GetComponent<LightComponent>().intensity = 3.0f;
        glm::vec4 color(
            static_cast<float>(j)/maxCol,
            static_cast<float>(i)/maxRow,
            1.0f,
            1.0f);
        l.GetComponent<LightComponent>().color = color;

        //auto g = scene->CreateEntity(std::to_string(i));
        //g.GetComponent<TransformComponent>().position = glm::vec3(i*3, j*3, 0.0f);
        ////g.GetComponent<TransformComponent>().scale = glm::vec3(0.001f, 0.001f, 0.001f);
        //g.AddComponent<VulkanMeshComponent>(
        //    //ASSETS + "/Models/primitives/sphere.fbx"
        //    //ASSETS + "/Models/ganon/scene.gltf"
        //    //ASSETS + "/Models/cyborg/cyborg.obj"
        //    //ASSETS + "/Models/nanosuit/nanosuit.obj"
        //    ASSETS + "/Models/helmet/scene.gltf"
        //    //ASSETS + "/Models/Avocado/glTF/Avocado.gltf"
        //    //ASSETS + "/Models/Sponza/glTF/Sponza.gltf"
        //);
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
                    sizeof(lights) + sizeof(glm::vec4),
                    environmentSets[i].get()
                )
            );
            BufferLayout lightLayout =
            {
                { ShaderDataType::FLOAT4, "pos" },
                { ShaderDataType::FLOAT4, "color" },
                { ShaderDataType::FLOAT3, "dir" },
                { ShaderDataType::FLOAT, "intensity" },
            };
            // TODO change to use a specified byte range value instead of using layout
            // which will fix the need to do n+1 size allocations for array data in buffers
            lightUBOs[i]->SetLayout(lightLayout, 1, numLights+1);
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
        light.pos       = glm::vec4(transformComponent.position, 0.0f);
        light.dir       = glm::vec3(0.0f);
        light.intensity = lightComponent.intensity;
        light.color     = lightComponent.color;
        lights[numLights++] = light;
    }
    lightUBOs[frameIdx]->SetData(lights.data(), 0, sizeof(lights));
    lightUBOs[frameIdx]->SetData(&numLights, sizeof(lights), sizeof(uint32_t));

    VulkanImGuiLayer* vkImguiLayer = Application::Get().imguiLayer;

    vkImguiLayer->Begin();
    OnImGuiRender();
    vkImguiLayer->End();

    for (size_t i = 0; i < 3; ++i)
    {
        // Render scene and imgui
        auto commandBuffer = VulkanRenderer::BeginScene(i);
            auto group = scene->registry.group<VulkanMeshComponent>(entt::get<TransformComponent, TagComponent>);
            for (auto entity : group)
            {
                static int count = 0;
                auto [transformComponent, meshComponent, tagComponent] = group.get<TransformComponent, VulkanMeshComponent, TagComponent>(entity);

                mPushConstBlock.model = transformComponent;
                commandBuffer.pushConstants(
                    VulkanContext::Get()->GetPipeline()->GetVulkanPipelineLayout(),
                    vk::ShaderStageFlagBits::eVertex,
                    0, sizeof(VulkanPipeline::TransformPushConstBlock),
                    &mPushConstBlock);

                // Draw mesh
                SharedPtr<vk::VulkanMesh> mesh = meshComponent;
                mesh->Draw(commandBuffer);

                count++;
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

    // TODO display all entities in registry
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
