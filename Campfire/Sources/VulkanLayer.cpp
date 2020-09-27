#include "VulkanLayer.h"
#include "Platform/Vulkan/VulkanContext.h"

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
    vertexBufferPtr = CreateSharedPtr<VulkanVertexBuffer>(vertices, sizeof(vertices));
    indexBufferPtr = CreateSharedPtr<VulkanIndexBuffer>(indices, sizeof(indices)/sizeof(uint32_t));
}

void VulkanLayer::OnDetach()
{
}

void VulkanLayer::OnUpdate(float timestep)
{
    VulkanContext::GetInstance()->DrawIndexed(vertexBufferPtr->GetBuffer(), indexBufferPtr->GetBuffer(), sizeof(indices)/sizeof(uint32_t));
}

void VulkanLayer::OnImGuiRender()
{
}

void VulkanLayer::OnEvent(Event& event)
{
}
