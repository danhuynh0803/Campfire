#pragma once

#include "Renderer/Texture.h"
#include <vulkan/vulkan.hpp>

class VulkanTexture2D : public Texture2D
{
public:
    VulkanTexture2D(const std::string& path);
    VulkanTexture2D(uint32_t width, uint32_t height);
    VulkanTexture2D(uint32_t textureID);

    virtual uint32_t GetWidth() const override { return mWidth; }
    virtual uint32_t GetHeight() const override { return mHeight; }
    virtual std::string GetName() const override { return mFilePath; }
    virtual void Bind(uint32_t unit = 0) const override;
    virtual void SetData(void* data, uint32_t size);


private:
    uint32_t mWidth, mHeight;
    vk::UniqueImage mImage;
    vk::UniqueDeviceMemory mImageMemory;
    vk::UniqueImageView mImageView;
    vk::ImageLayout mImageLayout;
    std::string mFilePath;
};