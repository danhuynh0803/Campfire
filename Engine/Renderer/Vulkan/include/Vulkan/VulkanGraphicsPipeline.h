#pragma once

#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>
#include "Core/Base.h"

class VulkanGraphicsPipeline
{
public:
    struct TransformPushConstBlock {
        glm::mat4 model;
    } mTransformPushConstBlock;

    VulkanGraphicsPipeline(
        const std::vector<std::vector<vk::DescriptorSetLayoutBinding>> & descriptorSetLayoutBindings
      , const std::vector<vk::PipelineShaderStageCreateInfo> & shaderStages
    );

    ~VulkanGraphicsPipeline() = default;
    void RecreatePipeline();

    std::vector<vk::UniqueDescriptorSetLayout> mDescriptorSetLayouts;
    vk::UniquePipeline mPipeline;
    vk::UniquePipelineLayout mPipelineLayout;

private:
    vk::Device mDevice;
};
