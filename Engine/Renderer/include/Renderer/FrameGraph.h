#pragma once

#include "Core/Base.h"
#include <vulkan/vulkan.hpp>
#include "Vulkan/VulkanPipeline.h"

#include <map>
#include <vector>

template <typename T>
using LabelMap = std::map<std::string, T>;

struct AttachmentInfo
{
    vk::Format format       = vk::Format::eUndefined;
    uint32_t samples        = 1;
};

struct RenderPassInfo
{
    AttachmentInfo& AddAttachment() {
        return attachments.emplace_back(AttachmentInfo());
    }
    std::vector<AttachmentInfo> attachments;
    vk::AttachmentReference depthStencilAttachment;
};

enum class RenderQueueFlagsBits
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
    /*
    vk::SubpassDescription& AddSubpass(const std::string& name)
    {
        auto& it = subpasses.find(name);
        if (it != subpasses.end()) {
            return it->second;
        }

        subpasses.emplace(name);
        return subpasses.find(name)->second;
    }
    */

    vk::RenderPass& Get() {
        return uniqueRenderPass.get();
    }

    void Prepare()
    {
        /*
        // Setup render pass
        std::array<vk::AttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };

        vk::RenderPassCreateInfo renderPassCreateInfo {};
        renderPassCreateInfo.flags = vk::RenderPassCreateFlags();
        renderPassCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        renderPassCreateInfo.pAttachments = attachments.data();
        renderPassCreateInfo.subpassCount = static_cast<uint32_t>(subpasses.size());
        renderPassCreateInfo.pSubpasses = subpasses.data();
        renderPassCreateInfo.dependencyCount = static_cast<uint32_t>(subpassDependencies.size());
        renderPassCreateInfo.pDependencies = subpassDependencies.data();

        uniqueRenderPass = mDevice.createRenderPassUnique(renderPassCreateInfo);
        */
    }

private:
    LabelMap<vk::SubpassDescription> subpasses;
    LabelMap<vk::SubpassDependency> subpassDependencies;
    vk::UniqueRenderPass uniqueRenderPass;
};

class FrameGraph
{
public:
    FrameGraph();
    ~FrameGraph() {}
    void CreateRenderFrameGraph();

    RenderPass& AddRenderPass(
        const std::string& label,
        RenderQueue queue
    );

    vk::RenderPass GetRenderPass(const std::string& label) {
        return mRenderPasses.at(label).get();
    }

    SharedPtr<cf::Pipeline> GetGraphicsPipeline(const std::string& label) {
        return mPipelines.at(label);
    }

    void ReconstructFrameGraph();

private:
    void CreateOpaque();
    void CreatePipelines();

private:
    LabelMap<vk::UniqueRenderPass> mRenderPasses;
    LabelMap<vk::DescriptorSetLayout> mDescriptorSetLayouts;
    LabelMap<vk::DescriptorSet> mDescriptorSets;
    LabelMap<vk::PipelineLayout> mPipelineLayouts;
    LabelMap<SharedPtr<cf::Pipeline>> mPipelines;
    //LabelMap<vk::SubpassDescription> mSubpasses;
    //LabelMap<vk::SubpassDependency> mSubpassDependencies;
    vk::Device mDevice;
};
