#include "Vulkan/VulkanUtil.h"
#include "Vulkan/VulkanContext.h"
#include "Core/Log.h"

namespace vkUtil {

uint32_t FindMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties) {

    vk::PhysicalDeviceMemoryProperties memProperties = VulkanContext::Get()->GetDevice()->GetVulkanPhysicalDevice().getMemoryProperties();

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }
}

void CreateBuffer(uint32_t size, vk::BufferUsageFlags usageFlags, vk::MemoryPropertyFlags propertyFlags, vk::UniqueBuffer& buffer, vk::UniqueDeviceMemory& bufferMemory)
{
    vk::Device device = VulkanContext::Get()->GetDevice()->GetVulkanDevice();

    vk::BufferCreateInfo bufferInfo;
    bufferInfo.flags = vk::BufferCreateFlags();
    bufferInfo.size = size;
    bufferInfo.usage = usageFlags;
    bufferInfo.sharingMode = vk::SharingMode::eExclusive;

    buffer = device.createBufferUnique(bufferInfo);

    // Set memory requirements
    vk::MemoryRequirements memoryReqs = device.getBufferMemoryRequirements(buffer.get());

    vk::MemoryAllocateInfo allocInfo;
    allocInfo.allocationSize = memoryReqs.size;
    allocInfo.memoryTypeIndex = FindMemoryType(memoryReqs.memoryTypeBits, propertyFlags);

    bufferMemory = device.allocateMemoryUnique(allocInfo);

    device.bindBufferMemory(buffer.get(), bufferMemory.get(), 0);
}

void CopyBuffer(vk::UniqueBuffer& srcBuffer, vk::UniqueBuffer& dstBuffer, uint32_t size)
{
    auto commandBuffer = BeginSingleTimeCommands();
        vk::BufferCopy copyRegion;
        copyRegion.srcOffset = 0;
        copyRegion.dstOffset = 0;
        copyRegion.size = size;
        commandBuffer.copyBuffer(
            srcBuffer.get(),
            dstBuffer.get(),
            1,
            &copyRegion
        );
    EndSingleTimeCommands(commandBuffer);
}

vk::CommandBuffer BeginSingleTimeCommands()
{
    auto cmdPool = VulkanContext::Get()->GetCommandPool();

    vk::CommandBufferAllocateInfo allocInfo {};
    allocInfo.level = vk::CommandBufferLevel::ePrimary;
    allocInfo.commandPool = cmdPool;
    allocInfo.commandBufferCount = 1;

    // Create a command buffer to process a one time submit command
    auto device = VulkanContext::Get()->GetDevice()->GetVulkanDevice();
    vk::CommandBuffer cmdBuffer;
    device.allocateCommandBuffers(&allocInfo, &cmdBuffer);

    vk::CommandBufferBeginInfo beginInfo {};
    beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
    cmdBuffer.begin(beginInfo);

    return cmdBuffer;
}

void EndSingleTimeCommands(vk::CommandBuffer commandBuffer)
{
    commandBuffer.end();

    vk::SubmitInfo submitInfo;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    // TODO maybe optimize by submitting and then batching all data later somehow?
    auto graphicsQueue = VulkanContext::Get()->GetDevice()->GetGraphicsQueue();
    graphicsQueue.submit(1, &submitInfo, nullptr);
    graphicsQueue.waitIdle();
}

void SwitchImageLayout(vk::Image image, vk::Format format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout)
{
    vk::ImageMemoryBarrier barrier;
    barrier.srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
    barrier.dstAccessMask = vk::AccessFlagBits::eMemoryRead;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = 0;
    barrier.dstQueueFamilyIndex = 0;
    barrier.image = image;
    barrier.subresourceRange = {
        vk::ImageAspectFlagBits::eColor,    // aspectMask
        0,                                  // baseMipLevel
        1,                                  // levelCount
        0,                                  // baseArrayLayer
        1                                   // layerCount
    };

    if (newLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal)
    {
        barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eDepth;
        if (HasStencilComponent(format))
        {
            barrier.subresourceRange.aspectMask |= vk::ImageAspectFlagBits::eStencil;
        }
    }
    else
    {
        barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    }

    vk::PipelineStageFlagBits srcStageFlags = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    vk::PipelineStageFlagBits dstStageFlags = vk::PipelineStageFlagBits::eBottomOfPipe;

    if (oldLayout == vk::ImageLayout::eUndefined
        && newLayout == vk::ImageLayout::eTransferDstOptimal)
    {
        barrier.srcAccessMask = vk::AccessFlagBits::eHostWrite;
        barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;
        srcStageFlags = vk::PipelineStageFlagBits::eTopOfPipe;
        dstStageFlags = vk::PipelineStageFlagBits::eTransfer;
    }
    else if (oldLayout == vk::ImageLayout::eTransferDstOptimal
            && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
    {
        barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
        barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
        srcStageFlags = vk::PipelineStageFlagBits::eTransfer;
        dstStageFlags = vk::PipelineStageFlagBits::eFragmentShader;
    }
    else if (oldLayout == vk::ImageLayout::eUndefined
            && newLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal)
    {
        barrier.srcAccessMask = vk::AccessFlagBits::eHostWrite;
        barrier.dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentRead
                              | vk::AccessFlagBits::eDepthStencilAttachmentWrite;
        srcStageFlags = vk::PipelineStageFlagBits::eTopOfPipe;
        dstStageFlags = vk::PipelineStageFlagBits::eEarlyFragmentTests;
    }
    else
    {
        // TODO: how to handle a layout that doesn't match a preset spec?
        // maybe just use the general flags?
        //CORE_WARN("Image layout transition does not match, using default values");
    }

    auto cmdBuffer = BeginSingleTimeCommands();
        cmdBuffer.pipelineBarrier(
            srcStageFlags,
            dstStageFlags,
            vk::DependencyFlagBits::eViewLocalKHR,
            0, nullptr,
            0, nullptr,
            1, &barrier
        );
    EndSingleTimeCommands(cmdBuffer);
}

vk::UniqueImage CreateUniqueImage(
    uint32_t width, uint32_t height,
    vk::Format format,
    vk::ImageTiling tiling,
    vk::ImageUsageFlags usage
)
{
    vk::ImageCreateInfo imageInfo;
    imageInfo.imageType = vk::ImageType::e2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;

    imageInfo.format = format;
    imageInfo.tiling = tiling;
    // eUndefined causes texels to be discared at first transition
    imageInfo.initialLayout = vk::ImageLayout::eUndefined;
    imageInfo.usage = usage;
    imageInfo.sharingMode = vk::SharingMode::eExclusive;
    // Multisampling spec, TODO so that images can be used with attachements
    imageInfo.samples = vk::SampleCountFlagBits::e1;

    auto device = VulkanContext::Get()->GetDevice()->GetVulkanDevice();
    return device.createImageUnique(imageInfo);
}

vk::UniqueDeviceMemory CreateUniqueDeviceMemory(
    vk::Image image,
    vk::MemoryPropertyFlags properties
)
{
    auto device = VulkanContext::Get()->GetDevice()->GetVulkanDevice();
    vk::MemoryRequirements memReqs = device.getImageMemoryRequirements(image);

    vk::MemoryAllocateInfo allocInfo;
    allocInfo.allocationSize = memReqs.size;
    allocInfo.memoryTypeIndex = FindMemoryType(memReqs.memoryTypeBits, properties);

    return device.allocateMemoryUnique(allocInfo);
}

vk::UniqueImageView CreateUniqueImageView(vk::Image image, vk::Format format, vk::ImageAspectFlagBits aspectFlags)
{
    vk::ImageViewCreateInfo createInfo {};
    createInfo.image = image;
    createInfo.viewType = vk::ImageViewType::e2D;
    createInfo.format = format;

    vk::ImageSubresourceRange subresourceRange;
    subresourceRange.aspectMask     = aspectFlags;
    subresourceRange.baseMipLevel   = 0;
    subresourceRange.levelCount     = 1;
    subresourceRange.baseArrayLayer = 0;
    subresourceRange.layerCount     = 1;
    createInfo.subresourceRange = subresourceRange;

    auto device = VulkanContext::Get()->GetDevice()->GetVulkanDevice();

    return device.createImageViewUnique(createInfo);
}

vk::Format FindSupportedFormat(
    const std::vector<vk::Format>& formats,
    vk::ImageTiling tiling,
    vk::FormatFeatureFlagBits features
)
{
    auto physicalDevice = VulkanContext::Get()->GetDevice()->GetVulkanPhysicalDevice();

    for (vk::Format format : formats)
    {
        auto props = physicalDevice.getFormatProperties(format);

        if (tiling == vk::ImageTiling::eLinear
            && (props.linearTilingFeatures & features) == features)
        {
            return format;
        }
        else if (tiling == vk::ImageTiling::eOptimal
            && (props.optimalTilingFeatures & features) == features)
        {
            return format;
        }
    }

    throw std::runtime_error("Failed to find supported format");
}

vk::Format FindDepthFormat()
{
    return FindSupportedFormat(
        { vk::Format::eD32Sfloat , vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint },
        vk::ImageTiling::eOptimal,
        vk::FormatFeatureFlagBits::eDepthStencilAttachment
    );
}

bool HasStencilComponent(vk::Format format)
{
    return format == vk::Format::eD32SfloatS8Uint || format == vk::Format::eD24UnormS8Uint;
}

}
