#ifndef OPENGL_TEXTURE_H
#define OPENGL_TEXTURE_H

#include "Renderer/Texture.h"

class OpenGLTexture2D : public Texture2D
{
public:
    OpenGLTexture2D(const std::string& path);
    OpenGLTexture2D(uint32_t _width, uint32_t _height);
    virtual ~OpenGLTexture2D();

    virtual uint32_t GetWidth() const override { return width; }
    virtual uint32_t GetHeight() const override { return height; }
    virtual void Bind(uint32_t unit = 0) const override;
    virtual void SetData(void* data, uint32_t size);

private:
    uint32_t renderID;
    uint32_t width, height;
    std::string filepath;
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
    // TODO
    virtual void SetData(void* data, uint32_t size) {}

private:
    uint32_t renderID;
    uint32_t width, height;
    std::vector<std::string> filepathList;
};

#endif // OPENGL_TEXTURE_H
