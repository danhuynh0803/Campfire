#pragma once

#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>
#include <map>

#include "Core/Base.h"

enum class PipelineType
{
    GRAPHICS,
    COMPUTE
};

class VulkanPipeline
{
public:
    static SharedPtr<VulkanPipeline> Create(PipelineType pipelineType);
    virtual ~VulkanPipeline() {}
    virtual void RecreatePipeline() = 0;

    std::vector< std::vector<vk::UniqueDescriptorSet> > mDescriptorSets;
    std::vector<vk::UniqueDescriptorSetLayout> mDescriptorSetLayouts;
    vk::UniquePipeline mPipeline;
    vk::UniquePipelineLayout mPipelineLayout;

    // Move to context?
    vk::UniqueDescriptorPool mDescriptorPool;
    // Move elsewhere of pipeline
    vk::UniqueRenderPass mRenderPass;
};

class GraphicsPipeline : public VulkanPipeline
{
public:
    struct TransformPushConstBlock {
        glm::mat4 model;
    } mTransformPushConstBlock;

    GraphicsPipeline();
    ~GraphicsPipeline() = default;
    virtual void RecreatePipeline() override;

private:
    void CreateDescriptorSets();
    void CreateDescriptorPool();
    void SetupDescriptors();
    void SetupRenderPass();
};

class ComputePipeline : public VulkanPipeline
{
public:
    ComputePipeline();
    ~ComputePipeline() = default;
    virtual void RecreatePipeline() override;
};
