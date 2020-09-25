#pragma once
#include <vulkan/vulkan.hpp>
#include "Renderer/Buffer.h"

#include <glm/glm.hpp>

struct Vertex
{
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 uv;

    static vk::VertexInputBindingDescription GetBindingDescription()
    {
        vk::VertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = vk::VertexInputRate::eVertex;

        return bindingDescription;
    }

    static std::array<vk::VertexInputAttributeDescription, 3> GetAttributeDescriptions()
    {
        std::array<vk::VertexInputAttributeDescription, 3> attributeDescriptions{};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = vk::Format::eR32G32B32Sfloat;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);

        attributeDescriptions[1].binding = 1;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = vk::Format::eR32G32B32Sfloat;
        attributeDescriptions[1].offset = offsetof(Vertex, color);

        attributeDescriptions[2].binding = 2;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = vk::Format::eR32G32Sfloat;;
        attributeDescriptions[2].offset = offsetof(Vertex, uv);

        return attributeDescriptions;
    }
};

//class VulkanVertexBuffer : public VertexBuffer
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

    vk::UniqueBuffer buffer;

private:
    uint32_t renderID;
    BufferLayout layout;
};

//class VulkanIndexBuffer : public IndexBuffer
//{
//
//};
//
//class VulkanUniformBuffer : public UniformBuffer
//{
//
//};
