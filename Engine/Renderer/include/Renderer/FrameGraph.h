#pragma once

#include "Core/Base.h"
#include <vulkan/vulkan.hpp>
#include "Vulkan/VulkanPipeline.h"
#include "Vulkan/VulkanUtil.h"
#include "Vulkan/VulkanContext.h"

#include <map>
#include <vector>
#include <optional>

template <typename T>
using LabelMap = std::map<std::string, T>;

// TODO
//enum Format
//{
//    None = 0,
//    R8G8B8_UNORM,
//    R8G8B8_SNORM,
//    R8G8B8_SFLOAT,
//};

// TODO Replace string with enum for static checking to avoid tag typos?
// Tags easier once rendergraph implementation completed
enum RenderPassType
{
    DepthOnly = 0,
    Gbuffer,
    Lighting,
    FowardOpaque,
    PostProcess,
};

struct AttachmentDescription
{
    vk::Format format = vk::Format::eUndefined;
    uint32_t samples  = 1;
    VkAttachmentLoadOp loadOp;
    VkAttachmentStoreOp storeOp;
    VkAttachmentLoadOp stencilLoadOp;
    VkAttachmentStoreOp stencilStoreOp;
};

struct AttachmentInfo
{
    vk::Format format = vk::Format::eUndefined;
    uint32_t samples  = 1;
    uint32_t levels   = 1;
    uint32_t layers   = 1;
};

struct FramebufferAttachmentInfo
{
    uint32_t width, height;
    vk::Format format;
    vk::ImageUsageFlags usageFlags;
    vk::ImageAspectFlags aspectFlags;
    std::string tag;
    //vk::MemoryPropertyFlags memoryFlags;
};

struct FramebufferAttachment
{
    FramebufferAttachment() = default;
    FramebufferAttachment(const FramebufferAttachmentInfo& info);

    vk::UniqueImage image;
    vk::UniqueDeviceMemory memory;
    vk::UniqueImageView imageView;
};

class Framebuffer
{
public:
    Framebuffer(
        const std::vector<FramebufferAttachmentInfo>& infos
      , vk::RenderPass renderPass
      , vk::Extent2D dimensions
    );

    vk::ImageView GetAttachment(const std::string& tag)
    {
        auto it = mTagToView.find(tag);
        if (it != mTagToView.end())
        {
            return it->second;
        }
        CORE_ERROR("No attachment created for tag: {0}", tag);
        return VK_NULL_HANDLE;
    }

    std::vector<FramebufferAttachment> attachments;
    vk::Framebuffer Get() { return mFramebuffer.get(); }

private:
    std::unordered_map<std::string, vk::ImageView> mTagToView;
    vk::UniqueFramebuffer mFramebuffer;
    vk::RenderPass mRenderPass;
    uint32_t mWidth, mHeight;
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

class RenderPass;

class SubPass
{
public:
    void AddColorOutput(std::string tag, const AttachmentInfo& info)
    {
        mColorAttachments.emplace(tag, info);
    }

    void AddAttachmentInput(std::string tag, const AttachmentInfo& info)
    {
        mInputAttachments.emplace(tag, info);
    }

    void SetDepthStencilOutput(const AttachmentInfo& info)
    {
        mDepthStencilAttachment = info;
    }

private:
    LabelMap<AttachmentInfo> mInputAttachments;
    LabelMap<AttachmentInfo> mColorAttachments;
    LabelMap<AttachmentInfo> mResolveAttachments;
    LabelMap<AttachmentInfo> mPreserveAttachments;
    std::optional<AttachmentInfo> mDepthStencilAttachment;

    LabelMap<vk::SubpassDependency> subpassDependencies;

    friend class RenderPass;
};

class RenderPass
{
public:
    SubPass& AddSubpass(const std::string& label)
    {
        auto& it = mSubpasses.find(label);
        if (it != mSubpasses.end()) {
            return it->second;
        }

        mSubpasses.emplace(label, SubPass{});
        return mSubpasses.find(label)->second;
    }

    void RenderPass::Prepare();

    vk::RenderPass Get() {
        return mUniqueRenderPass.get();
    }

    vk::ImageView GetAttachment(const std::string& tag) {
        return mFramebuffer->GetAttachment(tag);
    }

private:
    LabelMap<AttachmentInfo> mAttachmentInfos;
    LabelMap<SubPass> mSubpasses;
    vk::UniqueRenderPass mUniqueRenderPass;
    SharedPtr<Framebuffer> mFramebuffer;
};

class FrameGraph
{
public:
    FrameGraph() = default;
    ~FrameGraph() = default;

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
    LabelMap<RenderPass> mRenderPasses;
    LabelMap<vk::UniqueRenderPass> tempRenderPasses;
    LabelMap<vk::DescriptorSetLayout> mDescriptorSetLayouts;
    LabelMap<vk::DescriptorSet> mDescriptorSets;
    LabelMap<vk::PipelineLayout> mPipelineLayouts;
    //LabelMap<vk::SubpassDescription> mSubpasses;
    //LabelMap<vk::SubpassDependency> mSubpassDependencies;
    vk::Device mDevice;
};
