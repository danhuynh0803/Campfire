#include "OpenGLTexture.h"
#include "Core/Log.h"

#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

OpenGLTexture::OpenGLTexture(const std::string& path)
    : filepath(path)
{
    // TODO move this out into some resource manager to rewrite slashes for windows
    //std::replace(texturePath.begin(), texturePath.end(), '\\', '/');

    glGenTextures(1, &renderID);

    int nrChannels, width, height;
    unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    this->width = width;
    this->height = height;

    // TODO some textures don't need to be flipped
    // Figure out a way to handle this
    stbi_set_flip_vertically_on_load(true);

    if (data)
    {
        GLenum format;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, renderID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glBindTexture(GL_TEXTURE_2D, 0);
    }
    else
    {
        LOG_WARN("Failed to load: {0}", filepath);
    }

    stbi_image_free(data);
}

OpenGLTexture::~OpenGLTexture()
{
    glDeleteTextures(1, &renderID);
}

void OpenGLTexture::Bind(uint32_t unit) const
{
    glBindTextureUnit(unit, renderID);
}
