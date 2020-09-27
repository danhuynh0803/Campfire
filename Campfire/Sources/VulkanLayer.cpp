#include "VulkanLayer.h"
#include "Platform/Vulkan/VulkanContext.h"
#include "Core/Input.h"
#include "Scene/CameraController.h"

static SharedPtr<Camera> editorCamera;
static CameraController cameraController;

VulkanLayer::VulkanLayer()
    : Layer("VulkanLayer")
{
}

float vertices[] = {
    // Position           // Color            // UV
    -0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 1.0f,   0.0f, 1.0f,
     0.5f, -0.5f, 0.0f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f,
     0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 1.0f,   1.0f, 0.0f,
    -0.5f,  0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
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
        glm::vec3(0.0f, 5.0f, 10.0f),
        glm::vec3(-20.0f, 0.0f, 0.0f)
    );

    BufferLayout uboLayout =
    {
        { ShaderDataType::MAT4, "view" },
        { ShaderDataType::MAT4, "proj" },
        { ShaderDataType::MAT4, "viewProj" },
    };
    uniformBufferPtr = UniformBuffer::Create();
    uniformBufferPtr->SetLayout(uboLayout, 0);

    vertexBufferPtr = CreateSharedPtr<VulkanVertexBuffer>(vertices, sizeof(vertices));
    indexBufferPtr = CreateSharedPtr<VulkanIndexBuffer>(indices, sizeof(indices)/sizeof(uint32_t));
}

void VulkanLayer::OnDetach()
{
}

void VulkanLayer::OnUpdate(float dt)
{
    cameraController.OnUpdate(dt);
    // Update UBO from camera

    VulkanContext::GetInstance()->DrawIndexed(vertexBufferPtr->GetBuffer(), indexBufferPtr->GetBuffer(), sizeof(indices)/sizeof(uint32_t));
}

void VulkanLayer::OnImGuiRender()
{
}

void VulkanLayer::OnEvent(Event& event)
{
    cameraController.OnEvent(event);
}
