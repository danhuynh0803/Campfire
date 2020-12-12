#pragma once

#include "Core/Base.h"
#include <glad/glad.h>

// TODO Framebuffer should raw gl types values
// But use for now for testing HDR
struct FramebufferSpec
{
    uint32_t width;
    uint32_t height;
    GLint internalFormat;
    uint32_t samples = 0;
};

struct Framebuffer
{
    virtual ~Framebuffer() {}
    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;

    virtual uint32_t GetRenderID() const = 0;
    virtual uint32_t GetColorAttachmentID() const = 0;
    virtual uint32_t GetDepthAttachmentID() const = 0;
    virtual void Resize(const FramebufferSpec& spec, bool forceRecreate = false) = 0;

    static SharedPtr<Framebuffer> Create(const FramebufferSpec& spec);
};
