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
    vk::Format format = vk::Format::eUndefined;
    uint32_t samples  = 1;

    vk::Image image;
    vk::DeviceMemory memory;
    vk::ImageView imageView;
};

struct Framebuffer
{
    std::vector<AttachmentInfo> attachments;
    uint32_t width;
    uint32_t height;
    uint32_t layers;
    vk::RenderPass renderPass;
    vk::Framebuffer framebuffer;
};

struct RenderPassInfo
{
    AttachmentInfo& AddAttachment() {
        return attachments.emplace_back(AttachmentInfo());
    }
    std::vector<AttachmentInfo> attachments;
    vk::AttachmentReference depthStencilAttachment;
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

struct SubPass
{
};

class RenderPass
{
public:
    vk::SubpassDescription& AddSubpass(const std::string& label)
    {
        auto& it = mSubpasses.find(label);
        if (it != mSubpasses.end()) {
            return it->second;
        }

        mSubpasses.emplace(label, vk::SubpassDescription{});
        return mSubpasses.find(label)->second;
    }

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
    int32_t width, height;
    vk::Sampler sampler;
    vk::DescriptorImageInfo descriptorImageInfo;

    LabelMap<vk::SubpassDescription> mSubpasses;
    LabelMap<vk::SubpassDependency> subpassDependencies;
    vk::UniqueRenderPass uniqueRenderPass;
};

class FrameGraph
{
public:
    FrameGraph();
    ~FrameGraph() {}
    void Prepare();

    RenderPass& AddRenderPass(
        const std::string& label,
        RenderQueue queue
    );

    vk::RenderPass GetRenderPass(const std::string& label) {
        return tempRenderPasses.at(label).get();
    }

    void ReconstructFrameGraph();

private:
    void CreateOpaque();
    //void CreatePipelines();
    //SharedPtr<cf::Pipeline> CreateModelPipeline();
    //SharedPtr<cf::Pipeline> CreatePostProcessPipeline();
    //SharedPtr<cf::Pipeline> CreateRaytracingComputePipeline();

private:
    LabelMap<RenderPass> mRenderPasses;
    LabelMap<vk::UniqueRenderPass> tempRenderPasses;
    LabelMap<vk::DescriptorSetLayout> mDescriptorSetLayouts;
    LabelMap<vk::DescriptorSet> mDescriptorSets;
    LabelMap<vk::PipelineLayout> mPipelineLayouts;
    //LabelMap<vk::SubpassDescription> mSubpasses;
    //LabelMap<vk::SubpassDependency> mSubpassDependencies;
    vk::Device mDevice;
};
