#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include "VulkanBuffer.h"
#include "VulkanContext.h"
#include <vulkan/vulkan.hpp>

uint32_t FindMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties) {

    vk::PhysicalDeviceMemoryProperties memProperties = VulkanContext::GetInstance()->GetPhysicalDevice().getMemoryProperties();

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }
}

void VulkanVertexBuffer::CreateBuffer(uint32_t size, vk::BufferUsageFlags usageFlags, vk::MemoryPropertyFlags propertyFlags, vk::UniqueBuffer& buffer, vk::UniqueDeviceMemory& bufferMemory)
{
    vk::Device device = VulkanContext::GetInstance()->GetDevice();

    vk::BufferCreateInfo bufferInfo
    {
        .flags = vk::BufferCreateFlags()
        , .size = size
        , .usage = usageFlags
        , .sharingMode = vk::SharingMode::eExclusive
    };

    buffer = device.createBufferUnique(bufferInfo);

    // Set memory requirements
    vk::MemoryRequirements memoryReqs = device.getBufferMemoryRequirements(buffer.get());

    vk::MemoryAllocateInfo allocInfo
    {
        .allocationSize = memoryReqs.size
        , .memoryTypeIndex = FindMemoryType(memoryReqs.memoryTypeBits, propertyFlags)
    };

    bufferMemory = device.allocateMemoryUnique(allocInfo);

    device.bindBufferMemory(buffer.get(), bufferMemory.get(), 0);
}

void VulkanVertexBuffer::CopyBuffer(vk::UniqueBuffer& srcBuffer, vk::UniqueBuffer& dstBuffer, uint32_t size)
{
    vk::CommandBufferAllocateInfo allocateInfo
    {
        .commandPool = VulkanContext::GetInstance()->GetCommandPool()
        , .level = vk::CommandBufferLevel::ePrimary
        , .commandBufferCount = 1
    };

    std::vector<vk::UniqueCommandBuffer> commandBuffer =
        VulkanContext::GetInstance()->GetDevice().allocateCommandBuffersUnique(allocateInfo);

    vk::CommandBufferBeginInfo beginInfo
    {
        .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit
    };

    commandBuffer[0]->begin(beginInfo);
        vk::BufferCopy copyRegion
        {
            .srcOffset = 0
            , .dstOffset = 0
            , .size = size
        };
        commandBuffer[0]->copyBuffer(srcBuffer.get(), dstBuffer.get(), 1, &copyRegion);
    commandBuffer[0]->end();

    vk::SubmitInfo submitInfo
    {
        .commandBufferCount = 1
        , .pCommandBuffers = &commandBuffer[0].get()
    };

    vk::Queue graphicsQueue = VulkanContext::GetInstance()->GetQueue();
    graphicsQueue.submit(1, &submitInfo, nullptr);
    graphicsQueue.waitIdle();
}

VulkanVertexBuffer::VulkanVertexBuffer(float* vertices, uint32_t size)
{
    vk::UniqueBuffer stagingBuffer;
    vk::UniqueDeviceMemory stagingBufferMemory;

    vk::BufferUsageFlags stagingUsage = vk::BufferUsageFlagBits::eTransferSrc;
    vk::MemoryPropertyFlags stagingMemoryProperties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
    CreateBuffer(size, stagingUsage, stagingMemoryProperties, stagingBuffer, stagingBufferMemory);

    // Copy data over
    vk::Device device = VulkanContext::GetInstance()->GetDevice();
    void* data = device.mapMemory(stagingBufferMemory.get(), 0, size, vk::MemoryMapFlags());
        memcpy(data, vertices, (size_t)size);
    device.unmapMemory(stagingBufferMemory.get());

    vk::BufferUsageFlags usage = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer;
    vk::MemoryPropertyFlags memoryProperties = vk::MemoryPropertyFlagBits::eDeviceLocal;
    CreateBuffer(size, usage, memoryProperties, buffer, bufferMemory);

    CopyBuffer(stagingBuffer, buffer, size);
}