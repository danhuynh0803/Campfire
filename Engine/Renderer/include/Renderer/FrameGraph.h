#pragma once

#include "Core/Base.h"
#include <vulkan/vulkan.hpp>
#include "Vulkan/VulkanPipeline.h"
#include "Vulkan/VulkanUtil.h"
#include "Vulkan/VulkanContext.h"

#include <map>
#include <vector>

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

    //std::string tag;
    //uint32_t memoryProperties = 0;
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

    FramebufferAttachment(const FramebufferAttachmentInfo& info)
    {
        auto device = VulkanContext::Get()->GetDevice()->GetVulkanDevice();
        constexpr uint32_t mipLevels = 1;
        image = vk::util::CreateUniqueImage(
            info.width,
            info.height,
            mipLevels,
            info.format,
            vk::ImageTiling::eOptimal,
            info.usageFlags
        );

        // Create image memory
        memory = vk::util::CreateUniqueDeviceMemory(
            image.get(),
            vk::MemoryPropertyFlagBits::eDeviceLocal // | vk::MemoryPropertyFlagBits::eHostVisible
        );
        device.bindImageMemory(image.get(), memory.get(), 0);

        imageView = vk::util::CreateUniqueImageView(
            image.get(),
            mipLevels,
            info.format,
            info.aspectFlags
        );
    }

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
    ) : mWidth(dimensions.width)
      , mHeight(dimensions.height)
      , mRenderPass(renderPass)
    {
        attachments.clear();
        for (const auto& info : infos)
        {
            // Check if width, height matches FB spec?
            // Maybe assert since we'd want them to match in most cases
            //info.width = dimensions.width;
            //info.height = dimensions.height;
            attachments.emplace_back(FramebufferAttachment(info));

            auto it = mTagToView.find(info.tag);
            if (it != mTagToView.end())
            {
                CORE_INFO("Replacing attachment at tag: {0}", info.tag);
            }

            mTagToView[info.tag] = attachments.back().imageView.get();
        }

        std::vector<vk::ImageView> views {};
        for (const auto& attachment : attachments)
        {
            views.emplace_back(attachment.imageView.get());
        }

        vk::FramebufferCreateInfo createInfo {};
        createInfo.renderPass = renderPass;
        createInfo.attachmentCount = views.size();
        createInfo.pAttachments = views.data();
        createInfo.width = dimensions.width;
        createInfo.height = dimensions.height;
        createInfo.layers = 1;

        auto device = VulkanContext::Get()->GetDevice()->GetVulkanDevice();
        mFramebuffer = device.createFramebufferUnique(createInfo);
    }

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
    //std::vector<FramebufferAttachment> colorAttachments;
    //FramebufferAttachment depthStencilAttachment;

private:
    std::unordered_map<std::string, vk::ImageView> mTagToView;
    vk::UniqueFramebuffer mFramebuffer;
    vk::RenderPass mRenderPass;
    uint32_t mWidth, mHeight;
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

class SubPass
{
public:
    void AddColorOutput(std::string tag, const AttachmentInfo& info)
    {

    }

    void AddAttachmentInput(std::string tag, const AttachmentInfo& info)
    {

    }

    void SetDepthStencilOutput(std::string tag, const AttachmentInfo& info)
    {

    }

    void AddTextureInput(std::string tag, const AttachmentInfo& info)
    {

    }

private:
    LabelMap<AttachmentInfo> mInputAttachments;
    LabelMap<AttachmentInfo> mColorAttachments;
    LabelMap<AttachmentInfo> mResolveAttachments;
    LabelMap<AttachmentInfo> mPreserveAttachments;
    AttachmentInfo mDepthStencilAttachment;

    LabelMap<vk::SubpassDependency> subpassDependencies;
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

    vk::RenderPass Get() {
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
    LabelMap<AttachmentInfo> mAttachmentInfos;
    LabelMap<SubPass> mSubpasses;
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
    void CreateOnScreenPass();
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
