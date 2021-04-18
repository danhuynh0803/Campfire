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

    VulkanGraphicsPipeline();
    ~VulkanGraphicsPipeline() = default;
    void RecreatePipeline();

    std::vector< std::vector<vk::UniqueDescriptorSet> > mDescriptorSets;
    std::vector<vk::UniqueDescriptorSetLayout> mDescriptorSetLayouts;
    vk::UniquePipeline mPipeline;
    vk::UniquePipelineLayout mPipelineLayout;

private:
    void CreateDescriptorSets();
    void CreateDescriptorPool();
    void SetupDescriptors();

    vk::Device mDevice;
};


