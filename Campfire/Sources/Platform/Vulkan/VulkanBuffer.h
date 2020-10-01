#pragma once
#include <vulkan/vulkan.hpp>
#include "Renderer/Buffer.h"

#include <glm/glm.hpp>

void CreateBuffer(uint32_t size, vk::BufferUsageFlags usageFlags, vk::MemoryPropertyFlags propertyFlags, vk::UniqueBuffer& buffer, vk::UniqueDeviceMemory& bufferMemory);

void CopyBuffer(vk::UniqueBuffer& srcBuffer, vk::UniqueBuffer& dstBuffer, uint32_t size);

class VulkanVertexBuffer
{
public:
    //VulkanVertexBuffer(uint32_t size);
    VulkanVertexBuffer(float* vertices, uint32_t size);
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
    VulkanIndexBuffer(uint32_t* indices, uint32_t count);

    vk::Buffer GetBuffer() { return buffer.get(); }

private:
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
    virtual void SetLayout(const BufferLayout& layout, uint32_t blockIndex, uint32_t count = 1) override;

    vk::Buffer GetBuffer() { return buffer.get(); }

private:
    vk::UniqueBuffer buffer;
    vk::UniqueDeviceMemory bufferMemory;
    vk::UniqueDescriptorSetLayout descriptorSetLayout;
};
