#include "Core/Base.h"
#include "Core/ResourceManager.h"

#include "Renderer/FrameGraph.h"

#include "Vulkan/VulkanContext.h"
#include "Vulkan/VulkanInitializers.h"
#include "Vulkan/VulkanShader.h"
#include "Vulkan/VulkanUtil.h"
#include "Vulkan/VulkanDevice.h"
#include "Vulkan/VulkanPipeline.h"
#include "Vulkan/VulkanSwapChain.h"

#include <vector>

FramebufferAttachment::FramebufferAttachment(const FramebufferAttachmentInfo& info)
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

Framebuffer::Framebuffer(
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

void RenderPass::Prepare()
{
    std::vector<vk::AttachmentDescription> attachmentDescs {};
    std::vector<FramebufferAttachmentInfo> fbAttachmentInfos {};
    std::vector<vk::SubpassDescription> subpasses {};
    std::vector<vk::AttachmentReference> colorRefs {};

    // TODO expose dimensions in RP creation
    uint32_t width  = VulkanContext::Get()->GetSwapChain()->GetWidth();
    uint32_t height = VulkanContext::Get()->GetSwapChain()->GetHeight();
    // iterate through all subpass attachments
    for (const auto& pairSubpass : mSubpasses)
    {
        const auto& subpass = pairSubpass.second;

        // color output attachments
        for (const auto& pairAttachment : subpass.mColorAttachments)
        {
            const auto& attachmentInfo = pairAttachment.second;

            vk::AttachmentDescription desc {};
            desc.format = attachmentInfo.format;
            // TODO incorporate uint to samples enum
            desc.samples = vk::SampleCountFlagBits::e1;
            desc.loadOp = vk::AttachmentLoadOp::eClear;
            // TODO options for transient?
            desc.storeOp = vk::AttachmentStoreOp::eStore;
            desc.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
            desc.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
            desc.initialLayout = vk::ImageLayout::eUndefined;
            // TODO generalize
            desc.finalLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
            attachmentDescs.emplace_back(desc);

            vk::AttachmentReference ref {};
            ref.attachment = colorRefs.size();
            ref.layout = vk::ImageLayout::eShaderReadOnlyOptimal;
            colorRefs.emplace_back(ref);

            // fb attachment info
            FramebufferAttachmentInfo fbInfo {};
            fbInfo.tag = pairAttachment.first;
            fbInfo.width = width;
            fbInfo.height = height;
            fbInfo.format = attachmentInfo.format;
            fbInfo.usageFlags = vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled;
            fbInfo.aspectFlags = vk::ImageAspectFlagBits::eColor;
            fbAttachmentInfos.emplace_back(fbInfo);
        }

        // depth output attachment
        vk::AttachmentReference depthRef {};
        if (subpass.mDepthStencilAttachment.has_value())
        {
            auto depthFormat = vk::util::FindDepthFormat();

            vk::AttachmentDescription desc {};
            desc.format = depthFormat;
            desc.samples = vk::SampleCountFlagBits::e1;
            desc.loadOp = vk::AttachmentLoadOp::eClear;
            desc.storeOp = vk::AttachmentStoreOp::eDontCare;
            desc.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
            desc.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
            desc.initialLayout = vk::ImageLayout::eUndefined;
            desc.finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
            attachmentDescs.emplace_back(desc);

            depthRef.attachment = attachmentDescs.size() - 1;
            depthRef.layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

            FramebufferAttachmentInfo fbInfo {};
            fbInfo.tag = "depth";
            fbInfo.width = width;
            fbInfo.height = height;
            fbInfo.format = depthFormat;
            fbInfo.usageFlags = vk::ImageUsageFlagBits::eDepthStencilAttachment;
            fbInfo.aspectFlags = vk::ImageAspectFlagBits::eDepth;
            fbAttachmentInfos.emplace_back(fbInfo);
        }

        vk::SubpassDescription spDesc {};
        spDesc.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
        spDesc.colorAttachmentCount = static_cast<uint32_t>(colorRefs.size());
        spDesc.pColorAttachments = colorRefs.data();
        spDesc.pDepthStencilAttachment = subpass.mDepthStencilAttachment.has_value()
            ? &depthRef
            : nullptr;
        subpasses.emplace_back(spDesc);
    }

    // TODO for now, handle just one subpass renderpasses
    std::vector<vk::SubpassDependency> subpassDependencies;
    {
        vk::SubpassDependency dep;
        dep.srcSubpass = VK_SUBPASS_EXTERNAL;
        dep.dstSubpass = 0;
        dep.srcStageMask = vk::PipelineStageFlagBits::eTopOfPipe;
        dep.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests;
        dep.srcAccessMask = vk::AccessFlagBits::eMemoryRead;
        dep.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite;
        subpassDependencies.emplace_back(dep);
    }

    // render pass
    vk::RenderPassCreateInfo renderPassCreateInfo {};
    renderPassCreateInfo.flags = vk::RenderPassCreateFlags();
    renderPassCreateInfo.attachmentCount = static_cast<uint32_t>(attachmentDescs.size());
    renderPassCreateInfo.pAttachments = attachmentDescs.data();
    renderPassCreateInfo.subpassCount = static_cast<uint32_t>(subpasses.size());
    renderPassCreateInfo.pSubpasses = subpasses.data();
    renderPassCreateInfo.dependencyCount = static_cast<uint32_t>(subpassDependencies.size());
    renderPassCreateInfo.pDependencies = subpassDependencies.data();

    auto device = VulkanContext::Get()->GetDevice()->GetVulkanDevice();
    mUniqueRenderPass = device.createRenderPassUnique(renderPassCreateInfo);

    // Now create our FB with associated imageviews
    mFramebuffer = CreateSharedPtr<Framebuffer>(
        fbAttachmentInfos,
        mUniqueRenderPass.get(),
        vk::Extent2D{width, height}
    );
}



void FrameGraph::Prepare()
{
    mDevice = VulkanContext::Get()->GetDevice()->GetVulkanDevice();

    for (auto& pair : mRenderPasses)
    {
        auto& rp = pair.second;
        rp.Prepare();
    }
}

RenderPass& FrameGraph::AddRenderPass(
    const std::string& label,
    RenderQueue queue)
{
    auto& it = mRenderPasses.find(label);
    if (it != mRenderPasses.end()) {
        return it->second;
    }

    mRenderPasses.emplace(label, RenderPass{});
    return mRenderPasses.find(label)->second;
}

void FrameGraph::ReconstructFrameGraph()
{
}
