#pragma once

#include <glad/glad.h>
#include "Renderer/Texture.h"

class OpenGLTexture2D : public Texture2D
{
public:
    OpenGLTexture2D(const std::string& path);
    OpenGLTexture2D(uint32_t _width, uint32_t _height);
    OpenGLTexture2D(uint32_t textureID);
    virtual ~OpenGLTexture2D();

    virtual uint32_t GetWidth() const override { return width; }
    virtual uint32_t GetHeight() const override { return height; }
    virtual std::string GetName() const override { return filepath; }
    virtual void Bind(uint32_t unit = 0) const override;
    virtual void SetData(void* data, uint32_t size);

    // Hacky way of just overriding the renderID so we can
    // update texture with raw gl calls
    virtual void SetData(uint32_t newRenderID) { renderID = newRenderID; }

    virtual uint32_t GetRenderID() const override { return renderID; }

private:
    uint32_t renderID;
    uint32_t width, height;
    std::string filepath;
    GLenum internalFormat, dataFormat;
};

// TODO
//class OpenGLTexture3D : public Texture3D
//{
//public:
//};


class OpenGLTextureCube : public TextureCube
{
public:
    //OpenGLTextureCube(const std::string& path);
    OpenGLTextureCube(const std::vector<std::string>& pathList);
    virtual ~OpenGLTextureCube();

    virtual uint32_t GetWidth() const override { return width; }
    virtual uint32_t GetHeight() const override { return height; }
    virtual void Bind(uint32_t unit = 0) const override;
    virtual void SetData(void* data, uint32_t size) {}
    virtual void SetData(uint32_t newRenderID) { renderID = newRenderID; }

    virtual std::string GetName() const override { return "TODO"; }

private:
    uint32_t renderID;
    uint32_t width, height;
    std::vector<std::string> filepathList;
};
