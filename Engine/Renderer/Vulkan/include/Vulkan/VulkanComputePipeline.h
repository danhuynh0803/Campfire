#pragma once

#include <vulkan/vulkan.hpp>

class VulkanComputePipeline
{
public:
    VulkanComputePipeline();
    ~VulkanComputePipeline() = default;
    void RecreatePipeline();

    std::vector<vk::UniqueDescriptorSet> mDescriptorSets;
    vk::UniqueDescriptorSetLayout mDescriptorSetLayout;
    vk::UniquePipeline mPipeline;
    vk::UniquePipelineLayout mPipelineLayout;
    vk::UniqueDescriptorPool mDescriptorPool;

private:
};
