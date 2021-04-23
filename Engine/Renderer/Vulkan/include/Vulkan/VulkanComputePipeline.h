#pragma once

#include <vulkan/vulkan.hpp>
#include "Vulkan/VulkanTexture.h"

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
    std::vector<vk::UniqueCommandBuffer> mCmdBuffers;
    vk::DescriptorImageInfo mDescriptorImageInfo;

    // TODO use textures to generate descriptors maybe?
    // so that the process of adding attachments can be
    // streamlined a bit more
    SharedPtr<VulkanTexture2D> mTexture;

private:
    vk::Device mDevice;
};
