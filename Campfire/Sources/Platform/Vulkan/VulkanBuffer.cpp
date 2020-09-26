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

void VulkanVertexBuffer::CreateBuffer(uint32_t size, vk::BufferUsageFlags usageFlags, vk::MemoryPropertyFlags propertyFlags, vk::UniqueBuffer& buffer, vk::UniqueDeviceMemory& bufferMemory)
{
    vk::BufferCreateInfo bufferInfo
    {
        .flags = vk::BufferCreateFlags()
        , .size = size
        , .usage = usageFlags
        , .sharingMode = vk::SharingMode::eExclusive
    };

    buffer = VulkanContext::GetDevice().createBufferUnique(bufferInfo);

    // Set memory requirements
    vk::MemoryRequirements memoryReqs = VulkanContext::GetDevice().getBufferMemoryRequirements(buffer.get());

    vk::MemoryAllocateInfo allocInfo
    {
        .allocationSize = memoryReqs.size
        , .memoryTypeIndex = FindMemoryType(memoryReqs.memoryTypeBits, propertyFlags)
    };

    bufferMemory = VulkanContext::GetDevice().allocateMemoryUnique(allocInfo);

    VulkanContext::GetDevice().bindBufferMemory(buffer.get(), bufferMemory.get(), 0);
}

VulkanVertexBuffer::VulkanVertexBuffer(float* vertices, uint32_t size)
{
    vk::MemoryPropertyFlags memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
    CreateBuffer(size, vk::BufferUsageFlagBits::eVertexBuffer, memoryProperties, buffer, bufferMemory);

    // Copy data over
    void* data = VulkanContext::GetDevice().mapMemory(bufferMemory.get(), 0, size, vk::MemoryMapFlags());
    memcpy(data, vertices, (size_t)size);
    VulkanContext::GetDevice().unmapMemory(bufferMemory.get());
}
