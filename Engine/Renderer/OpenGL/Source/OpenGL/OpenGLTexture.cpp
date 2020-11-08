#include "OpenGLTexture.h"
#include "Core/Log.h"

#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

//=====================================================
//------------------ Texture2D ------------------------
//=====================================================
OpenGLTexture2D::OpenGLTexture2D(uint32_t _width, uint32_t _height)
    : width(_width), height(_height)
{
    internalFormat = GL_RGBA8;
    dataFormat = GL_RGBA;

    glCreateTextures(GL_TEXTURE_2D, 1, &renderID);
    glTextureStorage2D(renderID, 1, internalFormat, width, height);

    glTexParameteri(renderID, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(renderID, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(renderID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(renderID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}


OpenGLTexture2D::OpenGLTexture2D(const std::string& path)
    : filepath(path)
{
    stbi_set_flip_vertically_on_load(true);

    // TODO move this out into some resource manager to rewrite slashes for windows
    //std::replace(texturePath.begin(), texturePath.end(), '\\', '/');

    glGenTextures(1, &renderID);

    int nrChannels, width, height;
    unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    this->width = width;
    this->height = height;

    if (data)
    {
        if (nrChannels == 4)
        {
            internalFormat = GL_RGBA8;
            dataFormat = GL_RGBA;
        }
        else if (nrChannels == 3)
        {
            internalFormat = GL_RGB8;
            dataFormat = GL_RGB;
        }
        else if (nrChannels == 1)
        {
            internalFormat = GL_RED;
            dataFormat = GL_RED;
        }

        glBindTexture(GL_TEXTURE_2D, renderID);
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);

        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glBindTexture(GL_TEXTURE_2D, 0);
    }
    else
    {
        LOG_ERROR("Failed to load: {0}", filepath);
    }

    stbi_image_free(data);
}

OpenGLTexture2D::~OpenGLTexture2D()
{
    glDeleteTextures(1, &renderID);
}

void OpenGLTexture2D::Bind(uint32_t unit) const
{
    glBindTextureUnit(unit, renderID);
}

void OpenGLTexture2D::SetData(void* data, uint32_t size)
{
    //uint32_t bpp = dataFormat == GL_RGBA ? 4 : 3;
    glTextureSubImage2D(renderID, 0, 0, 0, width, height, dataFormat, GL_UNSIGNED_BYTE, data);
}

//=====================================================
//------------------ Texture Cube ---------------------
//=====================================================
OpenGLTextureCube::OpenGLTextureCube(const std::vector<std::string>& pathList)
    : filepathList(pathList)
{
    stbi_set_flip_vertically_on_load(false);

    glGenTextures(1, &renderID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, renderID);

    int width, height, nrChannels;
    unsigned char* data;
    for (GLuint i = 0; i < filepathList.size(); ++i)
    {
        data = stbi_load(filepathList[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, GL_RGB, width, height, 0,
                GL_RGB, GL_UNSIGNED_BYTE, data
            );
        }
        else
        {
            LOG_ERROR("Cubemap texture failed to load: {0}", filepathList[i]);
        }
        stbi_image_free(data);
    }

    // Store dimensions of final face,
    // since they should all be the same dimensionsi anyway
    this->width = width;
    this->height = height;

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

}

OpenGLTextureCube::~OpenGLTextureCube()
{
    glDeleteTextures(1, &renderID);
}

void OpenGLTextureCube::Bind(uint32_t unit) const
{
    glBindTexture(GL_TEXTURE_CUBE_MAP, renderID);
}
