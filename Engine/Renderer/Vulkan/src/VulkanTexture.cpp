#include "Vulkan/VulkanTexture.h"
#include "Vulkan/VulkanUtil.h" // CreateBuffer, CopyBuffer, FindMemoryType
#include "Vulkan/VulkanContext.h" // VulkanContext
#include "Core/Log.h"

// Leave this commented out unless OpenGL is not built since
// #define STB_IMAGE_IMPLEMENTATION already set in OpenGLTexture.cpp
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
    mImage = CreateUniqueImage(
        static_cast<uint32_t>(width),
        static_cast<uint32_t>(height),
        vk::Format::eR8G8B8A8Srgb,
        vk::ImageTiling::eOptimal,
        vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled
    );

    mImageMemory = CreateUniqueDeviceMemory(
        mImage.get(),
        vk::MemoryPropertyFlagBits::eDeviceLocal
    );
    // Bind Image memory
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

    // Create ImageView
    mImageView = CreateUniqueImageView(
        mImage.get(),
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
        createInfo.minLod = 0.0f;
        createInfo.maxLod = 0.0f;
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
