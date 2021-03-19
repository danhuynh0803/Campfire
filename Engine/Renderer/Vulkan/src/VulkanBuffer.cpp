//#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include "Vulkan/VulkanBuffer.h"
#include "Vulkan/VulkanContext.h"
#include "Vulkan/VulkanUtil.h" // CreateBuffer, CopyBuffer, FindMemoryType
#include "Vulkan/VulkanInitializers.h"

VulkanBuffer::VulkanBuffer(
    vk::BufferUsageFlags usage,
    vk::MemoryPropertyFlags propertyFlags,
    uint32_t size,
    vk::SharingMode sharingMode)
    : mSize(size)
{
    mDevice = VulkanContext::Get()->GetDevice()->GetVulkanDevice();

    auto bufferInfo = vk::initializers::BufferCreateInfo(size, usage, sharingMode);
    mBuffer = mDevice.createBufferUnique(bufferInfo);

    vk::MemoryRequirements memoryReqs = mDevice.getBufferMemoryRequirements(mBuffer.get());

    vk::MemoryAllocateInfo allocInfo;
    allocInfo.allocationSize = memoryReqs.size;
    allocInfo.memoryTypeIndex = vk::util::FindMemoryType(memoryReqs.memoryTypeBits, propertyFlags);
    mBufferMemory = mDevice.allocateMemoryUnique(allocInfo);

    Bind();
}

void VulkanBuffer::Bind(vk::DeviceSize offset)
{
    mDevice.bindBufferMemory(mBuffer.get(), mBufferMemory.get(), offset);
}

void* VulkanBuffer::Map()
{
    mMappedRegion = mDevice.mapMemory(mBufferMemory.get(), 0, mSize, vk::MemoryMapFlags());
    return mMappedRegion;
}

void VulkanBuffer::Unmap()
{
    if (mMappedRegion)
    {
        mDevice.unmapMemory(mBufferMemory.get());
        mMappedRegion = nullptr;
    }
}

void VulkanBuffer::Flush()
{

}

//=====================================================================
//------------------------- Vertex Buffers ----------------------------
//=====================================================================
VulkanVertexBuffer::VulkanVertexBuffer(void* vertices, uint32_t size)
{
    vk::UniqueBuffer stagingBuffer;
    vk::UniqueDeviceMemory stagingBufferMemory;

    vk::BufferUsageFlags stagingUsage = vk::BufferUsageFlagBits::eTransferSrc;
    vk::MemoryPropertyFlags stagingMemoryProperties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
    vk::util::CreateBuffer(size, stagingUsage, stagingMemoryProperties, stagingBuffer, stagingBufferMemory);

    // Copy data over
    auto device = VulkanContext::Get()->GetDevice()->GetVulkanDevice();
    void* data = device.mapMemory(stagingBufferMemory.get(), 0, size, vk::MemoryMapFlags());
        memcpy(data, vertices, (size_t)size);
    device.unmapMemory(stagingBufferMemory.get());

    vk::BufferUsageFlags usage = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer;
    vk::MemoryPropertyFlags memoryProperties = vk::MemoryPropertyFlagBits::eDeviceLocal;
    vk::util::CreateBuffer(size, usage, memoryProperties, buffer, bufferMemory);

    vk::util::CopyBuffer(stagingBuffer, buffer, size);
}

//=====================================================================
//------------------------- Index Buffers -----------------------------
//=====================================================================
VulkanIndexBuffer::VulkanIndexBuffer(void* indices, uint32_t count)
{
    vk::UniqueBuffer stagingBuffer;
    vk::UniqueDeviceMemory stagingBufferMemory;

    uint32_t size = sizeof(uint32_t) * count;
    vk::BufferUsageFlags stagingUsage = vk::BufferUsageFlagBits::eTransferSrc;
    vk::MemoryPropertyFlags stagingMemoryProperties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
    vk::util::CreateBuffer(size, stagingUsage, stagingMemoryProperties, stagingBuffer, stagingBufferMemory);

    // Copy data over
    auto device = VulkanContext::Get()->GetDevice()->GetVulkanDevice();
    void* data = device.mapMemory(stagingBufferMemory.get(), 0, size, vk::MemoryMapFlags());
        memcpy(data, indices, (size_t)size);
    device.unmapMemory(stagingBufferMemory.get());

    vk::BufferUsageFlags usage = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer;
    vk::MemoryPropertyFlags memoryProperties = vk::MemoryPropertyFlagBits::eDeviceLocal;
    vk::util::CreateBuffer(size, usage, memoryProperties, buffer, bufferMemory);

    vk::util::CopyBuffer(stagingBuffer, buffer, size);
}

//=====================================================================
//------------------------- Uniform Buffers ---------------------------
//=====================================================================
VulkanUniformBuffer::VulkanUniformBuffer(uint32_t size, vk::DescriptorSet descriptorSet)
    : mDescriptorSet(descriptorSet)
{
    vk::BufferUsageFlags usage = vk::BufferUsageFlagBits::eUniformBuffer;
    vk::MemoryPropertyFlags memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
    vk::util::CreateBuffer(size, usage, memoryProperties, buffer, bufferMemory);
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
