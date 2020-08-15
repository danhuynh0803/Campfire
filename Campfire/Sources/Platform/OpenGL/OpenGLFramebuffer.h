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
    virtual uint32_t GetDepthAttachmentID() const override { return depthAttachmentID; }

    virtual void Resize(uint32_t width, uint32_t height, uint32_t samples = 0, bool forceRecreate = false) override;

private:
    uint32_t width, height;
    uint32_t renderID = 0;
    uint32_t colorAttachmentID = 0;
    uint32_t depthAttachmentID = 0;
};

#endif // OPENGL_FRAMEBUFFER_H
