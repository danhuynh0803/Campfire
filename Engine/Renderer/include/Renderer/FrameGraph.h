#pragma once

#include <map>
#include <vulkan/vulkan.hpp>

#include <Vulkan/VulkanGraphicsPipeline.h>
#include <Vulkan/VulkanComputePipeline.h>

class FrameGraph
{
public:
    void CreateRenderFrameGraph();
    vk::Device mDevice;

    vk::RenderPass GetRenderPass(const std::string& label) { return mRenderPasses.at(label).get(); }
    SharedPtr<VulkanGraphicsPipeline> GetGraphicsPipeline(const std::string& label) { return mGraphicsPipelines.at(label); }

    void ReconstructFrameGraph();

private:
    void CreateOpaque();
    void PrepareGraphicsPipeline();

    template <typename T>
    using LabelMap = std::map<std::string, T>;
    LabelMap<vk::UniqueRenderPass> mRenderPasses;
    LabelMap<vk::DescriptorSetLayout> mDescriptorSetLayouts;
    LabelMap<vk::DescriptorSet> mDescriptorSets;
    LabelMap<vk::PipelineLayout> mPipelineLayouts;
    LabelMap<SharedPtr<VulkanGraphicsPipeline>> mGraphicsPipelines;
    LabelMap<VulkanComputePipeline> mComputePipelines;

    //LabelMap<vk::Pipeline> mPipelines;
    //LabelMap<vk::SubpassDescription> mSubpasses;
    //LabelMap<vk::SubpassDependency> mSubpassDependencies;
};
