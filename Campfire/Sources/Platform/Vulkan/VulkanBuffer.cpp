#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include "VulkanBuffer.h"
#include "VulkanContext.h"
#include <vulkan/vulkan.hpp>

uint32_t FindMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties) {

    vk::PhysicalDeviceMemoryProperties memProperties = VulkanContext::GetPhysicalDevice().getMemoryProperties();

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }
}

VulkanVertexBuffer::VulkanVertexBuffer(float* vertices, uint32_t size)
{
    vk::BufferCreateInfo bufferInfo
    {
        .flags = vk::BufferCreateFlags()
        , .size = size
        , .usage = vk::BufferUsageFlagBits::eVertexBuffer
        , .sharingMode = vk::SharingMode::eExclusive
    };

    buffer = VulkanContext::GetDevice().createBufferUnique(bufferInfo);

    // Set memory requirements
    vk::MemoryRequirements memoryReqs = VulkanContext::GetDevice().getBufferMemoryRequirements(buffer.get());

    vk::MemoryPropertyFlags flags = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
    vk::MemoryAllocateInfo allocInfo
    {
        .allocationSize = memoryReqs.size
        , .memoryTypeIndex = FindMemoryType(memoryReqs.memoryTypeBits, flags)
    };

    bufferMemory = VulkanContext::GetDevice().allocateMemoryUnique(allocInfo);

    VulkanContext::GetDevice().bindBufferMemory(buffer.get(), bufferMemory.get(), 0);

    void* data = VulkanContext::GetDevice().mapMemory(bufferMemory.get(), 0, bufferInfo.size, vk::MemoryMapFlags());
    memcpy(data, vertices, (size_t)bufferInfo.size);
    VulkanContext::GetDevice().unmapMemory(bufferMemory.get());
}
