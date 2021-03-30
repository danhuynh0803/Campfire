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

void VulkanLayer::OnAttach()
{
    // initial scene
    scene = CreateSharedPtr<Scene>();
    auto player = scene->CreateEntity("Player");
    player.GetComponent<TransformComponent>().position = glm::vec3(-1.0f, 0.0f, 0.0f);
    //player.AddComponent<VulkanMeshComponent>(ASSETS + "/Models/helmet/scene.gltf");

    auto e = scene->CreateEntity("E");
    e.GetComponent<TransformComponent>().position = glm::vec3(1.0f, 0.0f, 0.0f);
    //e.AddComponent<VulkanMeshComponent>(ASSETS + "/Models/helmet/scene.gltf");

    editorCamera = CreateSharedPtr<Camera>(1600, 900, 0.1f, 1000.0f);
    editorCamera->nearPlane = 0.001f;
    editorCamera->farPlane = 10000.0f;
    cameraController.normalSpeed = 15;
    cameraController.SetActiveCamera(
        editorCamera,
        glm::vec3(0.0f, 0.0f, 10.0f), // position
        glm::vec3(0.0f, 0.0f, 0.0f) // euler angles
    );

    //meshPtr = CreateSharedPtr<vk::VulkanMesh>(ASSETS + "/Models/primitives/sphere.fbx");
    //meshPtr = CreateSharedPtr<vk::VulkanMesh>(ASSETS + "/Models/ganon/scene.gltf");
    //meshPtr = CreateSharedPtr<vk::VulkanMesh>(ASSETS + "/Models/cyborg/cyborg.obj");
    //meshPtr = CreateSharedPtr<vk::VulkanMesh>(ASSETS + "/Models/nanosuit/nanosuit.obj");
    //meshPtr = CreateSharedPtr<vk::VulkanMesh>(ASSETS + "/Models/helmet/scene.gltf");
    meshPtr = CreateSharedPtr<vk::VulkanMesh>(ASSETS + "/Models/Sponza/glTF/Sponza.gltf");

    auto& descriptorSets = VulkanContext::Get()->GetPipeline()->uniformDescriptorSets;

    // TODO match with swapchainImages size
    for (size_t i = 0; i < 3; ++i)
    {
        { // Create camera UBOs
            cameraUBOs.emplace_back(
                CreateSharedPtr<VulkanUniformBuffer>(
                    sizeof(CameraUBO),
                    descriptorSets[i].get()
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

        { // Create Transform UBOs
            transformUBOs.emplace_back(
                CreateSharedPtr<VulkanUniformBuffer>(
                    sizeof(TransformUBO),
                    descriptorSets[i].get()
                )
            );
            BufferLayout transformLayout =
            {
                { ShaderDataType::MAT4, "model" },
            };
            transformUBOs[i]->SetLayout(transformLayout, 1);
        }

        { // Create Light UBOs
            lightUBOs.emplace_back(
                CreateSharedPtr<VulkanUniformBuffer>(
                    sizeof(LightUBO),
                    descriptorSets[i].get()
                )
            );
            BufferLayout lightLayout =
            {
                { ShaderDataType::FLOAT4, "pos" },
                { ShaderDataType::FLOAT4, "color" },
                { ShaderDataType::FLOAT4, "dir" },
            };
            lightUBOs[i]->SetLayout(lightLayout, 2);
        }

        { // Setup texture binding
            //textures.emplace_back(CreateSharedPtr<VulkanTexture2D>(ASSETS + "/Textures/awesomeface.png"));
            //textures[i]->UpdateDescriptors(descriptorSets[i].get(), 2);
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
    transformUBO.model = model;
    transformUBOs[frameIdx]->SetData(&transformUBO, 0, sizeof(TransformUBO));

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
            // Draw mesh
            meshPtr->Draw(commandBuffer);

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

    if (ImGui::Button("Load Mesh"))
    {
        std::string path = FileSystem::OpenFile();
        if (path.compare("") != 0) // No file selected
        {
            meshPtr.reset(new vk::VulkanMesh(path));
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
