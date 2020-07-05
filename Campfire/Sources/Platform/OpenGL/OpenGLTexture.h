#ifndef OPENGL_TEXTURE_H
#define OPENGL_TEXTURE_H

#include "Renderer/Texture.h"

class OpenGLTexture : public Texture
{
public:
    OpenGLTexture(const std::string& path);
    virtual ~OpenGLTexture();

    virtual uint32_t GetWidth() const override { return width; }
    virtual uint32_t GetHeight() const override { return height; }
    virtual void Bind(uint32_t unit = 0) const override;

private:
    uint32_t renderID;
    uint32_t width, height;
    std::string filepath;
};

#endif // OPENGL_TEXTURE_H
