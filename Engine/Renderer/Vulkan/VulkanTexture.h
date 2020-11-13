#pragma once

#include "Renderer/Texture.h"
#include <vulkan/vulkan.hpp>

class VulkanTexture2D : public Texture2D
{
public:
    VulkanTexture2D(const std::string& path);
private:
    uint32_t mWidth, mHeight;
    vk::Image mImage;
    vk::ImageLayout mImageLayout;
    std::string mFilePath;
};
