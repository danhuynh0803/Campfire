#pragma once

#include <vulkan/vulkan.hpp>

class VulkanComputePipeline
{
public:
    VulkanComputePipeline();
    ~VulkanComputePipeline() = default;
    void RecreatePipeline();

    std::vector< std::vector<vk::UniqueDescriptorSet> > mDescriptorSets;
    std::vector<vk::UniqueDescriptorSetLayout> mDescriptorSetLayouts;
    vk::UniquePipeline mPipeline;
    vk::UniquePipelineLayout mPipelineLayout;
    vk::UniqueDescriptorPool mDescriptorPool;
};
