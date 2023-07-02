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

void FrameGraph::Prepare()
{
    mDevice = VulkanContext::Get()->GetDevice()->GetVulkanDevice();

}

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
