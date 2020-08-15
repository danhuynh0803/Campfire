#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "Core/Base.h"

struct Framebuffer
{
    virtual ~Framebuffer() {}
    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;

    virtual uint32_t GetRenderID() const = 0;
    virtual uint32_t GetColorAttachmentID() const = 0;
    virtual uint32_t GetDepthAttachmentID() const = 0;

    virtual void Resize(uint32_t width, uint32_t height, uint32_t samples = 0, bool forceRecreate = false) = 0;

    static SharedPtr<Framebuffer> Create(uint32_t width, uint32_t height, uint32_t samples = 0);
};

#endif // FRAMEBUFFER_H
