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
    std::string tag;

    uint32_t memoryProperties = 0;
};

struct FramebufferSpec
{
    uint32_t width, height;
    vk::Format format;
    vk::ImageUsageFlags usageFlags;
    vk::ImageAspectFlags aspectFlags;
    //vk::MemoryPropertyFlags memoryFlags;

    // TODO color vs depth conditions
};

struct FramebufferAttachment
{
    FramebufferAttachment() = default;

    FramebufferAttachment(const FramebufferSpec& info)
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

struct Framebuffer
{
    std::vector<FramebufferAttachment> colorAttachments;
    FramebufferAttachment depthStencilAttachment;
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

    void AddColorOutput(std::string tag, const AttachmentInfo& info);
    void AddAttachmentInput(std::string tag, const AttachmentInfo& info);
    void SetDepthStencilOutput(std::string tag, const AttachmentInfo& info);
    void AddTextureInput(std::string tag, const AttachmentInfo& info);

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
