#include "Vulkan/VulkanUtil.h"
#include "Vulkan/VulkanContext.h"
#include "Vulkan/VulkanInitializers.h"
#include "Core/Log.h"

#include <algorithm>

namespace vk::util
{
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

        auto bufferInfo = vk::initializers::BufferCreateInfo(usageFlags, vk::SharingMode::eExclusive, size);
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
        auto cmdPool = VulkanContext::Get()->GetCommandPool(QueueFamilyType::GRAPHICS);

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
        auto graphicsQueue = VulkanContext::Get()->GetDevice()->GetQueue(QueueFamilyType::GRAPHICS);
        graphicsQueue.submit(1, &submitInfo, nullptr);
        graphicsQueue.waitIdle();

        auto device = VulkanContext::Get()->GetDevice()->GetVulkanDevice();
        device.freeCommandBuffers(
            VulkanContext::Get()->GetCommandPool(QueueFamilyType::GRAPHICS),
            1, &commandBuffer);
    }

    void SwitchImageLayout(vk::Image image, uint32_t mipLevels, vk::Format format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout, vk::DependencyFlagBits dependencyFlagBits)
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
            vk::ImageAspectFlagBits::eColor, // aspectMask
            0,                               // baseMipLevel
            mipLevels,                       // levelCount
            0,                               // baseArrayLayer
            1                                // layerCount
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
            srcStageFlags = vk::PipelineStageFlagBits::eHost;
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
            srcStageFlags = vk::PipelineStageFlagBits::eHost;
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
                dependencyFlagBits,
                0, nullptr,
                0, nullptr,
                1, &barrier
            );
        EndSingleTimeCommands(cmdBuffer);
    }

    vk::UniqueImage CreateUniqueImage(
        uint32_t width, uint32_t height,
        uint32_t mipLevels,
        vk::Format format,
        vk::ImageTiling tiling,
        vk::ImageUsageFlags usage
    )
    {
        vk::ImageCreateInfo imageInfo {};
        imageInfo.imageType     = vk::ImageType::e2D;
        imageInfo.extent.width  = width;
        imageInfo.extent.height = height;
        imageInfo.extent.depth  = 1;
        imageInfo.mipLevels     = mipLevels;
        imageInfo.arrayLayers   = 1;
        imageInfo.format        = format;
        imageInfo.tiling        = tiling;
        // eUndefined causes texels to be discared at first transition
        imageInfo.initialLayout = vk::ImageLayout::eUndefined;
        imageInfo.usage         = usage;
        imageInfo.sharingMode   = vk::SharingMode::eExclusive;
        // Multisampling spec, TODO so that images can be used with attachements
        imageInfo.samples       = vk::SampleCountFlagBits::e1;

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

    vk::UniqueImageView CreateUniqueImageView(
        vk::Image image,
        uint32_t mipLevels,
        vk::Format format,
        vk::ImageAspectFlags aspectFlags
    )
    {
        vk::ImageViewCreateInfo createInfo {};
        createInfo.image    = image;
        createInfo.viewType = vk::ImageViewType::e2D;
        createInfo.format   = format;

        vk::ImageSubresourceRange subresourceRange {};
        subresourceRange.aspectMask     = aspectFlags;
        subresourceRange.baseMipLevel   = 0;
        subresourceRange.levelCount     = mipLevels;
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

    std::vector<vk::DescriptorSetLayout> ConvertUnique(
        const std::vector<vk::UniqueDescriptorSetLayout>& uniqueLayouts
    )
    {
        std::vector<vk::DescriptorSetLayout> layouts;
        for (auto& uniqueLayout : uniqueLayouts)
        {
            layouts.emplace_back(uniqueLayout.get());
        }

        return layouts;
    }

    vk::GraphicsPipelineCreateInfo CreateBasePipelineInfo()
    {
        // Input assembly
        auto inputAssemblyStateCreateInfo =
            vk::initializers::PipelineInputAssemblyStateCreateInfo(vk::PrimitiveTopology::eTriangleList, VK_FALSE);

        // Setup viewports and scissor rect
        auto swapChain = VulkanContext::Get()->GetSwapChain();
        auto swapChainExtent = swapChain->GetExtent();
        auto viewport =
            vk::initializers::Viewport(0.0f, 0.0f, swapChainExtent.width, swapChainExtent.height, 0.0f, 1.0);

        vk::Rect2D scissors;
        scissors.offset = VkOffset2D{0, 0};
        scissors.extent = swapChainExtent;

        auto viewportStateCreateInfo =
            vk::initializers::PipelineViewportStateCreateInfo(1, &viewport, 1, &scissors);

        // Setup rasterizer
        auto rasterizationStateCreateInfo =
            vk::initializers::PipelineRasterizationStateCreateInfo(
                    vk::PolygonMode::eFill,
                    vk::CullModeFlagBits::eNone,
                    vk::FrontFace::eCounterClockwise);

        // Multisampling
        auto multisampleCreateInfo =
            vk::initializers::PipelineMultisampleStateCreateInfo(vk::SampleCountFlagBits::e1);

        // Depth and stencil operators
        auto depthStencilCreateInfo =
            vk::initializers::PipelineDepthStencilStateCreateInfo(VK_TRUE, VK_TRUE, vk::CompareOp::eLess);

        // Setup color blending
        auto colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
        auto colorBlendAttachment = vk::initializers::PipelineColorBlendAttachmentState(colorWriteMask, VK_FALSE);
        auto colorBlendState = vk::initializers::PipelineColorBlendStateCreateInfo(1, &colorBlendAttachment);

        // Setup dynamic state - these can be changed without recreating the pipeline
        std::vector<vk::DynamicState> dynamicStates = { vk::DynamicState::eViewport, vk::DynamicState::eScissor, vk::DynamicState::eLineWidth };

        auto dynamicStateCreateInfo = vk::initializers::PipelineDynamicStateCreateInfo(static_cast<uint32_t>(dynamicStates.size()), dynamicStates.data());

        // Create Graphics pipeline
        vk::GraphicsPipelineCreateInfo pipelineCreateInfo {};
        pipelineCreateInfo.flags = vk::PipelineCreateFlags();
        pipelineCreateInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo;
        pipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
        pipelineCreateInfo.pRasterizationState = &rasterizationStateCreateInfo;
        pipelineCreateInfo.pMultisampleState = &multisampleCreateInfo;
        pipelineCreateInfo.pDepthStencilState = &depthStencilCreateInfo;
        pipelineCreateInfo.pColorBlendState = &colorBlendState;
        pipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;
        pipelineCreateInfo.subpass = 0;
        pipelineCreateInfo.basePipelineHandle = nullptr;
        pipelineCreateInfo.basePipelineIndex = -1;

        return pipelineCreateInfo;
    }
}
