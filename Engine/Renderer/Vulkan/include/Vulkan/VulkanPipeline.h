#pragma once

#include <vulkan/vulkan.hpp>
#include <vector>

namespace cf
{

enum PipelineType
{
    eGraphics = 0,
    eCompute = 1,
    eRaytracing = 2,
    Count
};

class Pipeline
{
public:
    struct TransformPushConstBlock {
        glm::mat4 model;
    } mTransformPushConstBlock;

public:
    Pipeline(
        const std::vector<std::vector<vk::DescriptorSetLayoutBinding>> & descriptorSetLayoutBindings
      , const std::vector<vk::PipelineShaderStageCreateInfo> & shaderStages
      , PipelineType type
    );

private:
    std::vector<vk::UniqueDescriptorSet> mDescriptorSets;
    std::vector<vk::UniqueDescriptorSetLayout> mDescriptorSetLayouts;
    vk::UniquePipelineLayout mPipelineLayout;
    vk::UniquePipeline mPipeline;
    vk::Device mDevice;
    PipelineType type;
};

}
