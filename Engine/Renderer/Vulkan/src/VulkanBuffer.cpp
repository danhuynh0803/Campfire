#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include "Vulkan/VulkanBuffer.h"
#include "Vulkan/VulkanContext.h"
#include <vulkan/vulkan.hpp>

uint32_t FindMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties) {

    vk::PhysicalDeviceMemoryProperties memProperties = VulkanContext::Get()->GetDevice()->GetVulkanPhysicalDevice().getMemoryProperties();

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }
}

void CreateBuffer(uint32_t size, vk::BufferUsageFlags usageFlags, vk::MemoryPropertyFlags propertyFlags, vk::UniqueBuffer& buffer, vk::UniqueDeviceMemory& bufferMemory)
{
    vk::Device device = VulkanContext::Get()->GetDevice()->GetVulkanDevice();

    vk::BufferCreateInfo bufferInfo
    {
        .flags = vk::BufferCreateFlags(),
        .size = size,
        .usage = usageFlags,
        .sharingMode = vk::SharingMode::eExclusive,
    };

    buffer = device.createBufferUnique(bufferInfo);

    // Set memory requirements
    vk::MemoryRequirements memoryReqs = device.getBufferMemoryRequirements(buffer.get());

    vk::MemoryAllocateInfo allocInfo
    {
        .allocationSize = memoryReqs.size,
        .memoryTypeIndex = FindMemoryType(memoryReqs.memoryTypeBits, propertyFlags),
    };

    bufferMemory = device.allocateMemoryUnique(allocInfo);

    device.bindBufferMemory(buffer.get(), bufferMemory.get(), 0);
}

void CopyBuffer(vk::UniqueBuffer& srcBuffer, vk::UniqueBuffer& dstBuffer, uint32_t size)
{
    auto devicePtr = VulkanContext::Get()->GetDevice();

    vk::CommandBufferAllocateInfo allocateInfo
    {
        .commandPool = VulkanContext::Get()->mSwapChain->GetCommandPool(),
        .level = vk::CommandBufferLevel::ePrimary,
        .commandBufferCount = 1,
    };

    std::vector<vk::UniqueCommandBuffer> commandBuffer =
        devicePtr->GetVulkanDevice().allocateCommandBuffersUnique(allocateInfo);

    vk::CommandBufferBeginInfo beginInfo
    {
        .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit
    };

    commandBuffer[0]->begin(beginInfo);
        vk::BufferCopy copyRegion
        {
            .srcOffset = 0,
            .dstOffset = 0,
            .size = size,
        };
        commandBuffer[0]->copyBuffer(srcBuffer.get(), dstBuffer.get(), 1, &copyRegion);
    commandBuffer[0]->end();

    vk::SubmitInfo submitInfo
    {
        .commandBufferCount = 1,
        .pCommandBuffers = &commandBuffer[0].get(),
    };

    vk::Queue graphicsQueue = devicePtr->GetGraphicsQueue();
    graphicsQueue.submit(1, &submitInfo, nullptr);
    graphicsQueue.waitIdle();
}

//=====================================================================
//------------------------- Vertex Buffers ----------------------------
//=====================================================================
VulkanVertexBuffer::VulkanVertexBuffer(float* vertices, uint32_t size)
{
    vk::UniqueBuffer stagingBuffer;
    vk::UniqueDeviceMemory stagingBufferMemory;

    vk::BufferUsageFlags stagingUsage = vk::BufferUsageFlagBits::eTransferSrc;
    vk::MemoryPropertyFlags stagingMemoryProperties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
    CreateBuffer(size, stagingUsage, stagingMemoryProperties, stagingBuffer, stagingBufferMemory);

    // Copy data over
    auto device = VulkanContext::Get()->GetDevice()->GetVulkanDevice();
    void* data = device.mapMemory(stagingBufferMemory.get(), 0, size, vk::MemoryMapFlags());
        memcpy(data, vertices, (size_t)size);
    device.unmapMemory(stagingBufferMemory.get());

    vk::BufferUsageFlags usage = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer;
    vk::MemoryPropertyFlags memoryProperties = vk::MemoryPropertyFlagBits::eDeviceLocal;
    CreateBuffer(size, usage, memoryProperties, buffer, bufferMemory);

    CopyBuffer(stagingBuffer, buffer, size);
}

//=====================================================================
//------------------------- Index Buffers -----------------------------
//=====================================================================
VulkanIndexBuffer::VulkanIndexBuffer(uint32_t* indices, uint32_t count)
{
    vk::UniqueBuffer stagingBuffer;
    vk::UniqueDeviceMemory stagingBufferMemory;

    uint32_t size = sizeof(uint32_t) * count;
    vk::BufferUsageFlags stagingUsage = vk::BufferUsageFlagBits::eTransferSrc;
    vk::MemoryPropertyFlags stagingMemoryProperties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
    CreateBuffer(size, stagingUsage, stagingMemoryProperties, stagingBuffer, stagingBufferMemory);

    // Copy data over
    auto device = VulkanContext::Get()->GetDevice()->GetVulkanDevice();
    void* data = device.mapMemory(stagingBufferMemory.get(), 0, size, vk::MemoryMapFlags());
        memcpy(data, indices, (size_t)size);
    device.unmapMemory(stagingBufferMemory.get());

    vk::BufferUsageFlags usage = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer;
    vk::MemoryPropertyFlags memoryProperties = vk::MemoryPropertyFlagBits::eDeviceLocal;
    CreateBuffer(size, usage, memoryProperties, buffer, bufferMemory);

    CopyBuffer(stagingBuffer, buffer, size);
}

//=====================================================================
//------------------------- Uniform Buffers ---------------------------
//=====================================================================
VulkanUniformBuffer::VulkanUniformBuffer(uint32_t size, vk::DescriptorSet descriptorSet)
    : mDescriptorSet(descriptorSet)
{
    vk::BufferUsageFlags usage = vk::BufferUsageFlagBits::eUniformBuffer;
    vk::MemoryPropertyFlags memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
    CreateBuffer(size, usage, memoryProperties, buffer, bufferMemory);
}

void VulkanUniformBuffer::Bind() const
{
}

void VulkanUniformBuffer::Unbind() const
{
}

void VulkanUniformBuffer::SetData(void* inputData, uint32_t offset, uint32_t size)
{
    auto device = VulkanContext::Get()->GetDevice()->GetVulkanDevice();

    void* data = device.mapMemory(bufferMemory.get(), offset, size, vk::MemoryMapFlags());
        memcpy(data, inputData, size);
    device.unmapMemory(bufferMemory.get());
}

void VulkanUniformBuffer::SetLayout(const BufferLayout& layout, uint32_t blockIndex, uint32_t count)
{
    vk::DescriptorBufferInfo bufferInfo {};
    bufferInfo.buffer = GetBuffer();
    bufferInfo.offset = 0;
    bufferInfo.range = layout.GetStride();

    vk::WriteDescriptorSet descriptorWrite {};
    descriptorWrite.dstSet = mDescriptorSet;
    descriptorWrite.dstBinding = blockIndex;
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorType = vk::DescriptorType::eUniformBuffer;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.pBufferInfo = &bufferInfo;

    VulkanContext::Get()->GetDevice()->GetVulkanDevice().updateDescriptorSets(1, &descriptorWrite, 0, nullptr);
}
