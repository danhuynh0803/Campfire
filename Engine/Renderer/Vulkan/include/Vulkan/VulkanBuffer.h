#pragma once
#include <vulkan/vulkan.hpp>
#include "Renderer/Buffer.h"

#include <glm/glm.hpp>

class VulkanBuffer
{
public:
    VulkanBuffer(
        uint32_t size,
        vk::BufferUsageFlags usage,
        vk::MemoryPropertyFlags propertyFlags,
        vk::SharingMode sharingMode
    );

    void* Map();
    void Unmap();
    vk::Buffer Get() { return mBuffer.get(); }
private:
    vk::UniqueBuffer mBuffer;
    vk::UniqueDeviceMemory mBufferMemory;
    void* mMappedRegion;
    uint32_t mSize;
    vk::Device mDevice;
};

class VulkanVertexBuffer
{
public:
    //VulkanVertexBuffer(uint32_t size);
    VulkanVertexBuffer(void* vertices, uint32_t size);
    //virtual ~VulkanVertexBuffer();

    //virtual void Bind() const override;
    //virtual void Unbind() const override;
    //virtual void SetData(void* data, uint32_t size) override;
    //virtual void SetLayout(const BufferLayout& _layout) override { layout = _layout; }
    //virtual const BufferLayout& GetLayout() const override { return layout; }

    vk::Buffer GetBuffer() { return buffer.get(); }

private:
    vk::UniqueBuffer buffer;
    vk::UniqueDeviceMemory bufferMemory;
    //uint32_t renderID;
    //BufferLayout layout;
};

class VulkanIndexBuffer
{
public:
    VulkanIndexBuffer(void* indices, uint32_t count);

    vk::Buffer GetBuffer() { return buffer.get(); }

private:
    vk::UniqueBuffer buffer;
    vk::UniqueDeviceMemory bufferMemory;
};

class VulkanUniformBuffer : public UniformBuffer
{
public:
    VulkanUniformBuffer() = default;
    VulkanUniformBuffer(uint32_t size, vk::DescriptorSet descriptorSet);
    virtual ~VulkanUniformBuffer() {}
    virtual void Bind() const override;
    virtual void Unbind() const override;
    virtual void SetData(void* data, uint32_t offset, uint32_t size) override;
    virtual void SetLayout(const BufferLayout& layout, uint32_t blockIndex, uint32_t count = 1) override;

    vk::Buffer GetBuffer() { return buffer.get(); }

private:
    vk::UniqueBuffer buffer;
    vk::UniqueDeviceMemory bufferMemory;
    vk::DescriptorSet mDescriptorSet;
};
