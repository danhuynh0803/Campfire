#ifndef OPENGL_FRAMEBUFFER_H
#define OPENGL_FRAMEBUFFER_H

#include "Renderer/Framebuffer.h"

class OpenGLFramebuffer : public Framebuffer
{
public:
    OpenGLFramebuffer(const FramebufferSpec& spec);
    virtual ~OpenGLFramebuffer();

    virtual void Bind() const override;
    virtual void Unbind() const override;
    virtual uint32_t GetRenderID() const override { return mRenderID; }
    virtual uint32_t GetColorAttachmentID(int index = 0) const override { return mColorAttachmentIDs[index]; }
    virtual uint32_t GetDepthAttachmentID() const override { return mDepthAttachmentID; }

    virtual void Resize(const FramebufferSpec& spec, bool forceRecreate = false) override;

private:
    uint32_t mWidth, mHeight;
    uint32_t mRenderID = 0;
    uint32_t mColorAttachmentIDs[8] { 0 };
    uint32_t mDepthAttachmentID = 0;
};

#endif // OPENGL_FRAMEBUFFER_H
