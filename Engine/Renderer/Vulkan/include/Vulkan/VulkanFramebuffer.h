#pragma once

#include <vulkan/vulkan.hpp>

struct FrameBufferInfo
{
    vk::Image image;
    vk::DeviceMemory memory;
    vk::ImageView imageView;
};

class VulkanFramebuffer
{
public:
    VulkanFramebuffer(uint32_t width, uint32_t height);

private:
    vk::UniqueFramebuffer framebuffer;
};
