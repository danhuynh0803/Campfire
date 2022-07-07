#pragma once
#include <vulkan/vulkan.hpp>
#include "Renderer/Buffer.h"

#include <glm/glm.hpp>

struct VulkanBuffer
{
    VulkanBuffer() = default;
    VulkanBuffer(
        vk::BufferUsageFlags usage,
        vk::MemoryPropertyFlags propertyFlags,
        vk::DeviceSize size,
        vk::SharingMode sharingMode = vk::SharingMode::eExclusive
    );

    void Bind(vk::DeviceSize offset = 0);
    void* Map();
    void Unmap();
    void Flush();
    vk::Buffer Get() { return mBuffer.get(); }

    vk::UniqueBuffer mBuffer;
    vk::UniqueDeviceMemory mBufferMemory;
    void* mMappedRegion = nullptr;
    uint32_t mSize;
    vk::Device mDevice;
};

class VulkanVertexBuffer : public VertexBuffer
{
public:
    //VulkanVertexBuffer(uint32_t size);
    VulkanVertexBuffer(void* vertices, uint32_t size);
    virtual ~VulkanVertexBuffer() {}

    virtual void Bind() const override {}
    virtual void Unbind() const override {}
    virtual void SetData(void* data, uint32_t size) override {}
    virtual void SetLayout(const BufferLayout& _layout) override {}
    virtual const BufferLayout& GetLayout() const override { BufferLayout layout; return layout; }

    vk::Buffer GetBuffer() { return buffer.get(); }

private:
    vk::UniqueBuffer buffer;
    vk::UniqueDeviceMemory bufferMemory;
    //uint32_t renderID;
    //BufferLayout layout;
};

class VulkanIndexBuffer : public IndexBuffer
{
public:
    VulkanIndexBuffer(void* indices, uint32_t count);

    virtual ~VulkanIndexBuffer() {}
    virtual void Bind() const {}
    virtual void Unbind() const {}
    virtual uint32_t GetCount() const { return mCount; }

    vk::Buffer GetBuffer() { return buffer.get(); }

private:
    uint32_t mCount;
    vk::UniqueBuffer buffer;
    vk::UniqueDeviceMemory bufferMemory;
};

class VulkanUniformBuffer : public UniformBuffer
{
public:
    VulkanUniformBuffer() = default;
    VulkanUniformBuffer(uint32_t size);
    virtual ~VulkanUniformBuffer() {}
    virtual void Bind() const override;
    virtual void Unbind() const override;
    virtual void SetData(void* data, uint32_t offset, uint32_t size) override;

    void UpdateDescriptorSet(vk::DescriptorSet dstSet, uint32_t blockIndex, uint32_t size);
    void UpdateDescriptorSet(vk::DescriptorSet dstSet, const BufferLayout& layout, uint32_t blockIndex, uint32_t count = 1);
    virtual void SetLayout(uint32_t blockIndex, uint32_t size) override;
    virtual void SetLayout(const BufferLayout& layout, uint32_t blockIndex, uint32_t count = 1) override;

    vk::Buffer GetBuffer() { return buffer.get(); }
    vk::DescriptorSet mDescriptorSet;

private:
    vk::UniqueBuffer buffer;
    vk::UniqueDeviceMemory bufferMemory;
};
