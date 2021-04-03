#pragma once

#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>

enum class PipelineType
{
    GRAPHICS,
    COMPUTE
};

class VulkanPipeline
{
public:
    struct TransformPushConstBlock {
        glm::mat4 model;
    } mTransformPushConstBlock;

    void RecreatePipeline();
    VulkanPipeline(PipelineType pipelineType);
    ~VulkanPipeline() = default;
    vk::Pipeline GetVulkanPipeline() { return pipeline.get(); }
    vk::PipelineLayout GetVulkanPipelineLayout() { return pipelineLayout.get(); }
    vk::RenderPass GetVulkanRenderPass() { return renderPass.get(); }
    vk::RenderPass GetImGuiRenderPass() { return mImGuiRenderPass.get(); }

    // TODO: figure out a better way to bind the descriptorSet during draw call
    // maybe have this handled behind the scenes in renderer?
    std::vector<vk::UniqueDescriptorSet> uniformDescriptorSets;
    std::vector<vk::UniqueDescriptorSet> materialDescriptorSets;
    std::vector<vk::UniqueDescriptorSet> transformDescriptorSets;
    vk::UniqueDescriptorSetLayout uboDescriptorSetLayout;
    vk::UniqueDescriptorSetLayout materialDescriptorSetLayout;
    vk::UniqueDescriptorSetLayout transformDescriptorSetLayout;
    vk::UniqueDescriptorPool descriptorPool;

private:
    void CreateGraphicsPipeline();
    void CreateComputePipeline();
    void CreateDescriptorSets();
    void CreateDescriptorPool();
    void SetupRenderPass();
    void SetupDescriptors();
    void SetupUboDescriptor();
    void SetupMaterialDescriptor();

private:
    vk::UniquePipeline pipeline;
    PipelineType type;

    vk::UniquePipelineLayout pipelineLayout;
    vk::UniqueRenderPass renderPass;
    vk::UniqueRenderPass mImGuiRenderPass;
};
