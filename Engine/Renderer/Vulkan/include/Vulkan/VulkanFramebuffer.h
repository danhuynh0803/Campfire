#pragma once

#include <vulkan/vulkan.hpp>

class VulkanFramebuffer
{
public:
    VulkanFramebuffer(uint32_t width, uint32_t height);

private:
    vk::UniqueFramebuffer framebuffer;
};
