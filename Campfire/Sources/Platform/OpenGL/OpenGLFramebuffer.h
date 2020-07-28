#ifndef OPENGL_FRAMEBUFFER_H
#define OPENGL_FRAMEBUFFER_H

#include "Renderer/Framebuffer.h"

class OpenGLFramebuffer : public Framebuffer
{
public:
    OpenGLFramebuffer(uint32_t width, uint32_t height, uint32_t samples);
    virtual ~OpenGLFramebuffer();

    virtual void Bind() const override;
    virtual void Unbind() const override;
    virtual uint32_t GetRenderID() const override { return renderID; }
    virtual uint32_t GetColorAttachmentID() const override { return colorAttachmentID; }

private:
    uint32_t renderID;
    uint32_t colorAttachmentID;
    uint32_t depthAttachmentID;
};

#endif // OPENGL_FRAMEBUFFER_H
