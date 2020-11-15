#pragma once

#include <vulkan/vulkan.hpp>

enum class PipelineType
{
    GRAPHICS,
    COMPUTE
};

class VulkanPipeline
{
public:
    void RecreatePipeline();
    VulkanPipeline(PipelineType pipelineType);
    ~VulkanPipeline() = default;
    vk::Pipeline GetVulkanPipeline() { return pipeline.get(); }
    vk::PipelineLayout GetVulkanPipelineLayout() { return pipelineLayout.get(); }
    vk::RenderPass GetVulkanRenderPass() { return renderPass.get(); }

    // TODO: figure out a better way to bind the descriptorSet during draw call
    // maybe have this handled behind the scenes in renderer?
    std::vector<vk::UniqueDescriptorSet> descriptorSets;

private:
    vk::UniquePipeline pipeline;
    PipelineType type;

    vk::UniqueDescriptorPool descriptorPool;
    vk::UniqueDescriptorSetLayout descriptorSetLayout;
    vk::UniquePipelineLayout pipelineLayout;
    vk::UniqueRenderPass renderPass;
};
