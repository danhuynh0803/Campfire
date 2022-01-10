#include "Vulkan/VulkanTexture.h"
#include "Vulkan/VulkanUtil.h" // CreateBuffer, CopyBuffer, FindMemoryType
#include "Vulkan/VulkanContext.h" // VulkanContext
#include "Core/Log.h"

// Leave this commented out unless OpenGL is not built since
// #define STB_IMAGE_IMPLEMENTATION already set in OpenGLTexture.cpp
#include <stb_image.h>

static void GenerateMipmaps(vk::Image& image, int32_t width, int32_t height, uint32_t mipLevels);

static uint32_t CalculateMaxMipLevel(uint32_t width, uint32_t height) {
    return static_cast<uint32_t>(std::floor(std::log2(std::max(width, height)))) + 1;
}

void VulkanTexture2D::SetData(void* data, uint32_t size)
{
}

VulkanTexture2D::VulkanTexture2D(uint32_t width, uint32_t height)
    : mWidth(width), mHeight(height)
{
    // TODO aspectMask and format should be parameterized in api
    mDevice = VulkanContext::Get()->GetDevice()->GetVulkanDevice();

    // Create image
    uint32_t mipLevels = CalculateMaxMipLevel(width, height);
    mImage = vk::util::CreateUniqueImage(
        static_cast<uint32_t>(width),
        static_cast<uint32_t>(height),
        mipLevels,
        vk::Format::eR8G8B8A8Unorm,
        vk::ImageTiling::eOptimal,
        vk::ImageUsageFlagBits::eStorage | vk::ImageUsageFlagBits::eSampled
    );

    // Create image memory
    mImageMemory = vk::util::CreateUniqueDeviceMemory(
        mImage.get(),
        vk::MemoryPropertyFlagBits::eDeviceLocal
    );
    mDevice.bindImageMemory(mImage.get(), mImageMemory.get(), 0);

    // Transition image layout
    vk::util::SwitchImageLayout(
        mImage.get(),
        mipLevels,
        vk::Format::eR8G8B8A8Unorm,
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::eGeneral,
        vk::DependencyFlagBits::eByRegion
    );

    // Create sampler
    vk::SamplerCreateInfo samplerInfo {};
    samplerInfo.magFilter = vk::Filter::eLinear;
    samplerInfo.minFilter = vk::Filter::eLinear;
    samplerInfo.mipmapMode = vk::SamplerMipmapMode::eLinear;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 0.0f;
    samplerInfo.addressModeU = vk::SamplerAddressMode::eRepeat;
    samplerInfo.addressModeV = vk::SamplerAddressMode::eRepeat;
    samplerInfo.addressModeW = vk::SamplerAddressMode::eRepeat;
    samplerInfo.maxAnisotropy = 1.0f;
    samplerInfo.borderColor = vk::BorderColor::eIntOpaqueBlack;
    samplerInfo.unnormalizedCoordinates = false;
    samplerInfo.compareEnable = false;
    samplerInfo.compareOp = vk::CompareOp::eNever;

    mSampler = mDevice.createSamplerUnique(samplerInfo);

    // Create ImageView
    mImageView = vk::util::CreateUniqueImageView(
        mImage.get(),
        1,
        vk::Format::eR8G8B8A8Unorm,
        vk::ImageAspectFlagBits::eColor
    );
}

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
    vk::DeviceSize imageSize = mWidth * mHeight * 4;

    vk::UniqueBuffer stagingBuffer;
    vk::UniqueDeviceMemory stagingBufferMemory;
    vk::BufferUsageFlagBits stagingUsage = vk::BufferUsageFlagBits::eTransferSrc;
    vk::MemoryPropertyFlags stagingMemoryProperties =
        vk::MemoryPropertyFlagBits::eHostVisible
      | vk::MemoryPropertyFlagBits::eHostCoherent
    ;

    vk::util::CreateBuffer(
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
    uint32_t mipLevels = CalculateMaxMipLevel(mWidth, mHeight);
    mImage = vk::util::CreateUniqueImage(
        static_cast<uint32_t>(mWidth),
        static_cast<uint32_t>(mHeight),
        mipLevels,
        vk::Format::eR8G8B8A8Srgb,
        vk::ImageTiling::eOptimal,
        vk::ImageUsageFlagBits::eTransferSrc
          | vk::ImageUsageFlagBits::eTransferDst
          | vk::ImageUsageFlagBits::eSampled
    );

    mImageMemory = vk::util::CreateUniqueDeviceMemory(
        mImage.get(),
        vk::MemoryPropertyFlagBits::eDeviceLocal
    );
    // Bind Image memory
    device.bindImageMemory(mImage.get(), mImageMemory.get(), 0);

    // Transition image to be optimal for transfering
    // which is vk::ImageLayout::eTransferDstOptimal
    vk::util::SwitchImageLayout(
        mImage.get(),
        mipLevels,
        vk::Format::eR8G8B8A8Srgb,
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::eTransferDstOptimal,
        vk::DependencyFlagBits::eByRegion
    );

    // Now ready to copy data from staging buffer to the image
    auto cmdBuffer = vk::util::BeginSingleTimeCommands();
        vk::BufferImageCopy region {};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;

        region.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;

        region.imageOffset = vk::Offset3D {0, 0, 0};
        region.imageExtent = vk::Extent3D {
            static_cast<uint32_t>(mWidth),
            static_cast<uint32_t>(mHeight),
            1
        };

        cmdBuffer.copyBufferToImage(
            stagingBuffer.get(),
            mImage.get(),
            vk::ImageLayout::eTransferDstOptimal,
            1,
            &region
        );
    vk::util::EndSingleTimeCommands(cmdBuffer);

    GenerateMipmaps(mImage.get(), width, height, mipLevels);

    // Create ImageView
    mImageView = vk::util::CreateUniqueImageView(
        mImage.get(),
        mipLevels,
        vk::Format::eR8G8B8A8Srgb,
        vk::ImageAspectFlagBits::eColor
    );

    // Create sampler
    {
        vk::SamplerCreateInfo createInfo {};
        createInfo.magFilter = vk::Filter::eLinear;
        createInfo.minFilter = vk::Filter::eLinear;
        createInfo.mipmapMode = vk::SamplerMipmapMode::eLinear;
        createInfo.mipLodBias = 0.0f;
        createInfo.minLod = 0;
        createInfo.maxLod = static_cast<float>(mipLevels);
        createInfo.addressModeU = vk::SamplerAddressMode::eRepeat;
        createInfo.addressModeV = vk::SamplerAddressMode::eRepeat;
        createInfo.addressModeW = vk::SamplerAddressMode::eRepeat;
        createInfo.anisotropyEnable = true;

        // Query max anisotropy limit of GPU
        auto physicalDevice = VulkanContext::Get()->GetDevice()->GetVulkanPhysicalDevice();
        auto properties = physicalDevice.getProperties();
        createInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;

        // The color return when sampling beython the image if using clamp to border address mode
        createInfo.borderColor = vk::BorderColor::eIntOpaqueBlack;
        createInfo.unnormalizedCoordinates = false;
        createInfo.compareEnable = false;
        createInfo.compareOp = vk::CompareOp::eAlways;

        mSampler = device.createSamplerUnique(createInfo);
    }

    // TODO submit image data to graphicsQueue
}

void VulkanTexture2D::UpdateDescriptors(vk::DescriptorSet dstSet, uint32_t dstBinding)
{
    // Submit texture data to descriptorSet
    vk::DescriptorImageInfo imageInfo{};
    imageInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
    imageInfo.imageView = mImageView.get();
    imageInfo.sampler = mSampler.get();

    vk::WriteDescriptorSet descriptorWrite {};
    descriptorWrite.dstSet = dstSet;
    descriptorWrite.dstBinding = dstBinding;
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorType = vk::DescriptorType::eCombinedImageSampler;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.pImageInfo = &imageInfo;

    VulkanContext::Get()->GetDevice()->GetVulkanDevice().updateDescriptorSets(1, &descriptorWrite, 0, nullptr);
}

void GenerateMipmaps(vk::Image& image, int32_t width, int32_t height, uint32_t mipLevels)
{
    auto cmdBuffer = vk::util::BeginSingleTimeCommands();

    int32_t mipWidth  = width;
    int32_t mipHeight = height;

    // First need to transition to transfer
    vk::ImageMemoryBarrier barrier {};
    barrier.image = image;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.subresourceRange.levelCount = 1;

    for (uint32_t i = 1; i < mipLevels; ++i)
    {
        barrier.subresourceRange.baseMipLevel = i - 1;
        barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
        barrier.newLayout = vk::ImageLayout::eTransferSrcOptimal;
        barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
        barrier.dstAccessMask = vk::AccessFlagBits::eTransferRead;

        cmdBuffer.pipelineBarrier(
            vk::PipelineStageFlagBits::eTransfer,
            vk::PipelineStageFlagBits::eTransfer,
            vk::DependencyFlagBits::eByRegion,
            0, nullptr,
            0, nullptr,
            1, &barrier
        );

        // Blit for mipmaps
        vk::ImageBlit blit {};
        blit.srcOffsets[0]                 = vk::Offset3D{0, 0, 0};
        blit.srcOffsets[1]                 = vk::Offset3D{mipWidth, mipHeight, 1};
        blit.srcSubresource.aspectMask     = vk::ImageAspectFlagBits::eColor;
        blit.srcSubresource.mipLevel       = i - 1;
        blit.srcSubresource.baseArrayLayer = 0;
        blit.srcSubresource.layerCount     = 1;

        blit.dstOffsets[0] = vk::Offset3D{0, 0, 0};
        blit.dstOffsets[1] = vk::Offset3D {
                                mipWidth  > 1 ? mipWidth  / 2 : 1,
                                mipHeight > 1 ? mipHeight / 2 : 1,
                                1
                             };
        blit.dstSubresource.aspectMask     = vk::ImageAspectFlagBits::eColor;
        blit.dstSubresource.mipLevel       = i;
        blit.dstSubresource.baseArrayLayer = 0;
        blit.dstSubresource.layerCount     = 1;

        cmdBuffer.blitImage(
            image,
            vk::ImageLayout::eTransferSrcOptimal,
            image,
            vk::ImageLayout::eTransferDstOptimal,
            1, &blit,
            vk::Filter::eLinear
        );

        // Now transition each mip level to shader read layout using the same barrier
        barrier.oldLayout     = vk::ImageLayout::eTransferSrcOptimal;
        barrier.newLayout     = vk::ImageLayout::eShaderReadOnlyOptimal;
        barrier.srcAccessMask = vk::AccessFlagBits::eTransferRead;
        barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

        cmdBuffer.pipelineBarrier(
            vk::PipelineStageFlagBits::eTransfer,
            vk::PipelineStageFlagBits::eFragmentShader,
            vk::DependencyFlagBits::eByRegion,
            0, nullptr,
            0, nullptr,
            1, &barrier
        );

        // Scale down the width and height for the next mipmap
        if (mipWidth  > 1) mipWidth  /= 2;
        if (mipHeight > 1) mipHeight /= 2;
    }

    // Also need to transition the final mip level from TransferDstOptimal to ShaderReadOnlyOptimal
    barrier.subresourceRange.baseMipLevel = mipLevels - 1;
    barrier.oldLayout     = vk::ImageLayout::eTransferDstOptimal;
    barrier.newLayout     = vk::ImageLayout::eShaderReadOnlyOptimal;
    barrier.srcAccessMask = vk::AccessFlagBits::eTransferRead;
    barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

    cmdBuffer.pipelineBarrier(
        vk::PipelineStageFlagBits::eTransfer,
        vk::PipelineStageFlagBits::eFragmentShader,
        vk::DependencyFlagBits::eByRegion,
        0, nullptr,
        0, nullptr,
        1, &barrier
    );

    vk::util::EndSingleTimeCommands(cmdBuffer);
}
