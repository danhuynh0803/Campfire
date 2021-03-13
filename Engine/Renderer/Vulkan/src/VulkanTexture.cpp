#include "Vulkan/VulkanTexture.h"
#include "Vulkan/VulkanUtil.h" // CreateBuffer, CopyBuffer, FindMemoryType
#include "Vulkan/VulkanContext.h" // VulkanContext
#include "Core/Log.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

VulkanTexture2D::VulkanTexture2D(const std::string& path)
    : mFilePath(path)
{
    int nrChannels, width, height;
    unsigned char *imageData = stbi_load(path.c_str(), &width, &height, &nrChannels, STBI_rgb_alpha);

    if (!imageData)
    {
        LOG_ERROR("Failed to load: {0}", path);
        return;
    }

    mWidth = width;
    mHeight = height;

    vk::DeviceSize imageSize = width * height * 4;

    vk::UniqueBuffer stagingBuffer;
    vk::UniqueDeviceMemory stagingBufferMemory;
    vk::BufferUsageFlagBits stagingUsage = vk::BufferUsageFlagBits::eTransferSrc;
    vk::MemoryPropertyFlags stagingMemoryProperties =
            vk::MemoryPropertyFlagBits::eHostVisible
        | vk::MemoryPropertyFlagBits::eHostCoherent
    ;

    CreateBuffer(
        imageSize,
        stagingUsage,
        stagingMemoryProperties,
        stagingBuffer,
        stagingBufferMemory
    );

    // Copy data from staging buffer
    auto device = VulkanContext::Get()->GetDevice()->GetVulkanDevice();
    void* data = device.mapMemory(stagingBufferMemory.get(), 0, imageSize, vk::MemoryMapFlags());
        memcpy(data, imageData, static_cast<size_t>(imageSize));
    device.unmapMemory(stagingBufferMemory.get());

    stbi_image_free(imageData);

    // Create the actual texture image
    vk::UniqueDeviceMemory textureImageMemory;
    vk::ImageCreateInfo imageInfo;
    imageInfo.imageType = vk::ImageType::e2D;
    imageInfo.extent.width = static_cast<uint32_t>(width);
    imageInfo.extent.height = static_cast<uint32_t>(height);
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;

    // TODO match the internal format of the image
    // hardcode to rgba for now
    imageInfo.format = vk::Format::eR8G8B8A8Srgb;
    imageInfo.tiling = vk::ImageTiling::eOptimal;
    // eUndefined causes texels to be discared at first transition
    imageInfo.initialLayout = vk::ImageLayout::eUndefined;
    imageInfo.usage = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled;
    imageInfo.sharingMode = vk::SharingMode::eExclusive;
    // Multisampling spec, TODO so that images can be used with attachements
    imageInfo.samples = vk::SampleCountFlagBits::e1;

    mImage.get() = device.createImage(imageInfo);

    vk::MemoryRequirements memReqs = device.getImageMemoryRequirements(mImage.get());

    vk::MemoryAllocateInfo allocInfo;
    allocInfo.allocationSize = memReqs.size;
    allocInfo.memoryTypeIndex = FindMemoryType(memReqs.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal);

    mImageMemory.get() = device.allocateMemory(allocInfo);
    device.bindImageMemory(mImage.get(), mImageMemory.get(), 0);

    // Transition image to be optimal for transfering
    // which is vk::ImageLayout::eTransferDstOptimal
    SwitchImageLayout(
        mImage.get(),
        vk::Format::eR8G8B8A8Srgb,
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::eTransferDstOptimal
    );

    // Now ready to copy data from staging buffer to the image
    auto cmdBuffer = BeginSingleTimeCommands();
        vk::BufferImageCopy region {};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;

        region.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;

        region.imageOffset = {0, 0, 0};
        region.imageExtent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height),
            1
        };

        cmdBuffer.copyBufferToImage(
            stagingBuffer.get(),
            mImage.get(),
            vk::ImageLayout::eTransferDstOptimal,
            1,
            &region
        );
    EndSingleTimeCommands(cmdBuffer);

    // After data has been copied over, switch layout so that image is readable by shaders
    SwitchImageLayout(
        mImage.get(),
        vk::Format::eR8G8B8A8Srgb,
        vk::ImageLayout::eTransferDstOptimal,
        vk::ImageLayout::eShaderReadOnlyOptimal
    );

    // TODO submit image data to graphicsQueue
}
