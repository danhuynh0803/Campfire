#include "VulkanLayer.h"

#include "Vulkan/VulkanContext.h"
#include "Vulkan/VulkanRenderer.h"
#include "Vulkan/VulkanTexture.h"
#include "Vulkan/VulkanUtil.h"
#include "Vulkan/VulkanMesh.h"

#include "Core/Input.h"
#include "Core/ResourceManager.h" // ASSETS dir
#include "Scene/CameraController.h"

#include <glm/gtc/matrix_transform.hpp>
#include "Core/FileSystem.h"

static SharedPtr<Camera> editorCamera;
static CameraController cameraController;

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

VulkanLayer::VulkanLayer()
    : Layer("VulkanLayer")
{
}

void VulkanLayer::OnAttach()
{
    editorCamera = CreateSharedPtr<Camera>(1600, 900, 0.1f, 1000.0f);
    cameraController.SetActiveCamera(
        editorCamera,
        glm::vec3(0.0f, 0.0f, 10.0f), // position
        glm::vec3(0.0f, 0.0f, 0.0f) // euler angles
    );

    meshPtr = CreateSharedPtr<vk::VulkanMesh>(ASSETS + "/Models/primitives/sphere.fbx");

    auto& descriptorSets = VulkanContext::Get()->GetPipeline()->descriptorSets;

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

        { // Setup texture binding
            textures.emplace_back(CreateSharedPtr<VulkanTexture2D>(ASSETS + "/Textures/awesomeface.png"));
            textures[i]->UpdateDescriptors(descriptorSets[i].get(), 2);
        }
    }
}

void VulkanLayer::OnDetach()
{
}

void VulkanLayer::OnUpdate(float dt)
{
    // Current frame in flight
    auto frameIdx = VulkanContext::Get()->GetSwapChain()->GetCurrentImageIndex();

    // Update camera
    cameraController.OnUpdate(dt);

    // Update transform ubo
    //static float rotation = 0;
    //rotation += 180 * dt;
    glm::mat4 model = glm::mat4(1.0f);
    //model = glm::rotate(model, glm::radians(rotation), glm::vec3(0, 0, 1));
    transformUBO.model = model;
    transformUBOs[frameIdx]->SetData(&transformUBO, 0, sizeof(TransformUBO));

    // Update camera ubo
    cameraUBO.view = editorCamera->GetViewMatrix();
    cameraUBO.proj = editorCamera->GetProjMatrix();
    cameraUBO.proj[1][1] *= -1;
    cameraUBO.viewProj = cameraUBO.proj * cameraUBO.view;
    cameraUBOs[frameIdx]->SetData(&cameraUBO, 0, sizeof(CameraUBO));

    VulkanImGuiLayer* vkImguiLayer = Application::Get().imguiLayer;

    vkImguiLayer->Begin();
    OnImGuiRender();
    vkImguiLayer->End();

    // Render scene and imgui
    auto commandBuffer = VulkanRenderer::BeginScene();

        // Draw mesh
        meshPtr->Draw(commandBuffer);

        vkImguiLayer->mImGuiImpl->DrawFrame(commandBuffer);
    VulkanRenderer::EndScene(commandBuffer);
}

void VulkanLayer::OnImGuiRender()
{
    ImGui::Begin("Controls");

    if (ImGui::Button("Load Mesh"))
    {
        std::string path = FileSystem::OpenFile();
        if (path.compare("") != 0) // No file selected
        {
            meshPtr = CreateSharedPtr<vk::VulkanMesh>(path);
        }
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
    editorCamera->width = e.GetWidth();
    editorCamera->height = e.GetHeight();
    editorCamera->SetProjection();

    VulkanContext::Get()->RecreateSwapChain();

    return false;
}
