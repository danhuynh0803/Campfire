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
    virtual vk::Pipeline GetVulkanPipeline() = 0;
    virtual vk::PipelineLayout GetVulkanPipelineLayout() = 0;
    virtual vk::RenderPass GetVulkanRenderPass() = 0;
    virtual vk::DescriptorSet GetDescriptorSet(uint32_t setIndex, uint32_t frameIndex) = 0;
    virtual vk::DescriptorSetLayout GetDescriptorSetLayout(uint32_t setIndex) = 0;
    virtual vk::DescriptorPool GetDescriptorPool() = 0;

private:
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
    virtual vk::Pipeline GetVulkanPipeline() override {
        return pipeline.get();
    }
    virtual vk::PipelineLayout GetVulkanPipelineLayout() override {
        return pipelineLayout.get();
    }
    virtual vk::RenderPass GetVulkanRenderPass() override {
        return renderPass.get();
    }
    virtual vk::DescriptorSet GetDescriptorSet(uint32_t setIndex, uint32_t frameIndex) override {
        return descriptorSets.at(setIndex).at(frameIndex).get();
    }
    virtual vk::DescriptorSetLayout GetDescriptorSetLayout(uint32_t setIndex) override {
        return descriptorSetLayouts.at(setIndex).get();
    }
    virtual vk::DescriptorPool GetDescriptorPool() override { return descriptorPool.get(); }

private:
    void CreateDescriptorSets();
    void CreateDescriptorPool();
    void SetupRenderPass();
    void SetupDescriptors();

private:
    vk::UniquePipeline pipeline;
    vk::UniquePipelineLayout pipelineLayout;
    vk::UniqueRenderPass renderPass;

    std::vector< std::vector<vk::UniqueDescriptorSet> > descriptorSets;
    std::vector<vk::UniqueDescriptorSetLayout> descriptorSetLayouts;
    vk::UniqueDescriptorPool descriptorPool;
};

class ComputePipeline : public VulkanPipeline
{
public:
    ComputePipeline();
    ~ComputePipeline() = default;

    virtual void RecreatePipeline() override;
    virtual vk::Pipeline GetVulkanPipeline() override {
        return pipeline.get();
    }
    virtual vk::PipelineLayout GetVulkanPipelineLayout() override {
        return pipelineLayout.get();
    }
    virtual vk::RenderPass GetVulkanRenderPass() override {
        return renderPass.get();
    }
    virtual vk::DescriptorSet GetDescriptorSet(uint32_t setIndex, uint32_t frameIndex) override {
        return descriptorSets.at(setIndex).at(frameIndex).get();
    }
    virtual vk::DescriptorSetLayout GetDescriptorSetLayout(uint32_t setIndex) override {
        return descriptorSetLayouts.at(setIndex).get();
    }
    virtual vk::DescriptorPool GetDescriptorPool() override { return descriptorPool.get(); }

private:
    vk::UniquePipeline pipeline;
    vk::UniquePipelineLayout pipelineLayout;
    vk::UniqueRenderPass renderPass;

    std::vector< std::vector<vk::UniqueDescriptorSet> > descriptorSets;
    std::vector<vk::UniqueDescriptorSetLayout> descriptorSetLayouts;
    vk::UniqueDescriptorPool descriptorPool;
};
