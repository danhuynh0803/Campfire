#pragma once

#include <map>
#include <vector>
#include <vulkan/vulkan.hpp>
#include "Vulkan/VulkanGraphicsPipeline.h"
#include "Vulkan/VulkanComputePipeline.h"

struct AttachmentInfo
{
    uint32_t samples;
};

struct RenderPassInfo
{
    std::vector<AttachmentInfo> attachments;
    uint32_t numAttachments = 0;
    uint32_t loadOp         = 0;
    uint32_t storeOp        = 0;
    uint32_t stencilLoadOp  = 0;
    uint32_t stencilStoreOp = 0;
};

enum RenderQueueFlagsBits
{
    eGraphics       = 1 << 0,
    eCompute        = 1 << 1,
    eAsyncGraphics  = 1 << 2,
    eAsyncCompute   = 1 << 3,
    Count = 4
};
using RenderQueue = uint32_t;

class RenderPass
{
    RenderPass& AddRenderPass(
        const std::string& label,
        RenderQueue queue);

    // TODO
};

class FrameGraph
{
public:
    FrameGraph();
    ~FrameGraph() {}
    void CreateRenderFrameGraph();
    vk::RenderPass GetRenderPass(const std::string& label) {
        return mRenderPasses.at(label).get();
    }
    SharedPtr<VulkanGraphicsPipeline> GetGraphicsPipeline(const std::string& label) {
        return mGraphicsPipelines.at(label);
    }

    void ReconstructFrameGraph();

private:
    void CreateOpaque();
    void PrepareGraphicsPipeline();

private:
    template <typename T>
    using LabelMap = std::unordered_map<std::string, T>;
    LabelMap<vk::UniqueRenderPass> mRenderPasses;
    LabelMap<vk::DescriptorSetLayout> mDescriptorSetLayouts;
    LabelMap<vk::DescriptorSet> mDescriptorSets;
    LabelMap<vk::PipelineLayout> mPipelineLayouts;
    LabelMap<SharedPtr<VulkanGraphicsPipeline>> mGraphicsPipelines;
    LabelMap<SharedPtr<VulkanComputePipeline>> mComputePipelines;
    //LabelMap<vk::Pipeline> mPipelines;
    //LabelMap<vk::SubpassDescription> mSubpasses;
    //LabelMap<vk::SubpassDependency> mSubpassDependencies;
    vk::Device mDevice;
};
