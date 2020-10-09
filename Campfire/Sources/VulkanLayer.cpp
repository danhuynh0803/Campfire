#include "VulkanLayer.h"
#include "Platform/Vulkan/VulkanContext.h"
#include "Platform/Vulkan/VulkanRenderer.h"
#include "Core/Input.h"
#include "Scene/CameraController.h"

#include <glm/gtc/matrix_transform.hpp>

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

float vertices[] = {
    // Position           // Color            // UV
    -3.0f,  3.0f, 0.0f,   0.0f, 1.0f, 1.0f,   0.0f, 1.0f,
    -3.0f, -3.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
     3.0f, -3.0f, 0.0f,   1.0f, 0.0f, 1.0f,   1.0f, 0.0f,
     3.0f,  3.0f, 0.0f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f,
};

uint32_t indices[] = {
    0, 1, 2,
    2, 3, 0,
};

void VulkanLayer::OnAttach()
{
    editorCamera = CreateSharedPtr<Camera>(1600, 900, 0.1f, 1000.0f);
    cameraController.SetActiveCamera(
        editorCamera,
        glm::vec3(0.0f, 0.0f, 10.0f), // position
        glm::vec3(0.0f, 0.0f, 0.0f) // euler angles
    );

    vertexBufferPtr = CreateSharedPtr<VulkanVertexBuffer>(vertices, sizeof(vertices));
    indexBufferPtr = CreateSharedPtr<VulkanIndexBuffer>(indices, sizeof(indices)/sizeof(uint32_t));

    auto& descriptorSets = VulkanContext::Get()->mGraphicsPipeline->descriptorSets;

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
    }
}

void VulkanLayer::OnDetach()
{
}

void VulkanLayer::OnUpdate(float dt)
{
    // Current frame in flight
    auto frameIdx = VulkanContext::Get()->mSwapChain->GetCurrentImageIndex();

    // Update camera
    cameraController.OnUpdate(dt);

    // Update transform ubo
    static float rotation = 0;
    rotation += 180 * dt;
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

    // Draw
    VulkanRenderer::DrawIndexed(vertexBufferPtr->GetBuffer(), indexBufferPtr->GetBuffer(), sizeof(indices)/sizeof(uint32_t));
}

void VulkanLayer::OnImGuiRender()
{
}

void VulkanLayer::OnEvent(Event& event)
{
    cameraController.OnEvent(event);
}
