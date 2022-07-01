#pragma once

#include <vulkan/vulkan.hpp>
#include <vector>
#include "Vulkan/VulkanInitializers.h"

enum class PipelineType
{
    eGraphics = 0,
    eCompute = 1,
    eRaytracing = 2,
    Count
};

namespace cf
{
class Pipeline
{
public:


public:
    Pipeline(
        const std::vector<std::vector<vk::DescriptorSetLayoutBinding>> & descriptorSetLayoutBindings
      , const std::vector<vk::PipelineShaderStageCreateInfo> & shaderStages
      , PipelineType type);

private:

    vk::UniquePipeline CreateGraphicsPipeline(
        const std::vector<std::vector<vk::DescriptorSetLayoutBinding>> & descriptorSetLayoutBindings
      , const std::vector<vk::PipelineShaderStageCreateInfo> & shaderStages);

    std::vector<vk::UniqueDescriptorSet> mDescriptorSets;
    std::vector<vk::UniqueDescriptorSetLayout> mDescriptorSetLayouts;
    vk::UniquePipelineLayout mPipelineLayout;
    vk::UniquePipeline mPipeline;
    vk::Device mDevice;
    PipelineType type;
};

}

//=============================================================================
// Graphics pipeline related
struct TransformPushConstBlock
{
    glm::mat4 model;
};

enum class VertexComponent
{
    Position,
    UV,
    Normal
};

struct Vertex
{
    glm::vec3 pos;
    glm::vec2 uv;
    glm::vec3 normal;

    static vk::PipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo;
    static vk::VertexInputBindingDescription vertexInputBindingDescription;
    static std::vector<vk::VertexInputAttributeDescription> vertexInputAttributeDescriptions;

    static vk::PipelineVertexInputStateCreateInfo* GetPipelineVertexInputState(const std::vector<VertexComponent>& components)
    {
        vertexInputBindingDescription = vk::initializers::VertexInputBindingDescription(0, sizeof(Vertex), vk::VertexInputRate::eVertex);
        vertexInputAttributeDescriptions.clear();
        uint32_t location = 0;
        for (auto component : components)
        {
            vertexInputAttributeDescriptions.emplace_back(Vertex::InputAttributeDescription(0, location, component));
            location++;
        }

        pipelineVertexInputStateCreateInfo = vk::initializers::PipelineVertexInputStateCreateInfo(
                1, &vertexInputBindingDescription,
                static_cast<uint32_t>(vertexInputAttributeDescriptions.size()),
                vertexInputAttributeDescriptions.data()
                );

        return &pipelineVertexInputStateCreateInfo;
    }

    static vk::VertexInputAttributeDescription InputAttributeDescription(
            uint32_t binding,
            uint32_t location,
            VertexComponent component)
    {
        switch (component)
        {
            case VertexComponent::Position:
                return vk::initializers::VertexInputAttributeDescription(binding, location, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, pos));
            case VertexComponent::UV:
                return vk::initializers::VertexInputAttributeDescription(binding, location, vk::Format::eR32G32Sfloat, offsetof(Vertex, uv));
            case VertexComponent::Normal:
                return vk::initializers::VertexInputAttributeDescription(binding, location, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, normal));
            default:
                CORE_ERROR("No valid Input Attribute for specified component");
        }
    }
};
