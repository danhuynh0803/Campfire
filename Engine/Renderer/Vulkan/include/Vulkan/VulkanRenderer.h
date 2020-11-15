#pragma once

#include <vulkan/vulkan.hpp>

class VulkanRenderer
{
public:
    static void DrawIndexed(vk::Buffer vertexBuffer, vk::Buffer indexBuffer, uint32_t count);
};
