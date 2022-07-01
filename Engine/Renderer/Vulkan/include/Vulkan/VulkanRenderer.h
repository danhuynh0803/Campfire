#pragma once

#include <vulkan/vulkan.hpp>

class CommandBuffer
{
    VkCommandBuffer cmd;
};

class RenderContext
{
    // camera, light, any other global info

    // scissor, viewport
};

class VulkanRenderer
{
public:
    static vk::CommandBuffer& BeginScene();
    static vk::CommandBuffer& BeginScene(uint32_t frame);
    static void EndScene(vk::CommandBuffer& cmdBuffer);
    static void DrawIndexed(vk::CommandBuffer& commandBuffer, vk::Buffer vertexBuffer, vk::Buffer indexBuffer, uint32_t count);
};
