#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include "VulkanBuffer.h"
#include "VulkanContext.h"
#include <vulkan/vulkan.hpp>

VulkanVertexBuffer::VulkanVertexBuffer(float* vertices, uint32_t size)
{
    vk::BufferCreateInfo bufferInfo
    {
        .flags = vk::BufferCreateFlags()
        , .size = size
        , .usage = vk::BufferUsageFlagBits::eVertexBuffer
        , .sharingMode = vk::SharingMode::eExclusive
    };

    buffer = VulkanContext::device->createBufferUnique(bufferInfo);
}
