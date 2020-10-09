#include "VulkanTexture.h"
#include <stb_image.h>

VulkanTexture2D::VulkanTexture2D(const std::string& path)
    : mFilePath(path)
{
    int nrChannels, width, height;
    unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

    if (data)
    {
        mWidth = width;
        mHeight = height;
    }

}
