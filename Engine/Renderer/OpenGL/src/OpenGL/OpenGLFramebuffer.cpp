#include <glad/glad.h>
#include "Core/Log.h"

#include "OpenGL/OpenGLFramebuffer.h"

//OpenGLFramebuffer::OpenGLFramebuffer(uint32_t width, uint32_t height, uint32_t samples)
OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferSpec& spec)
{
    Resize(spec, true);
}

OpenGLFramebuffer::~OpenGLFramebuffer()
{
    glDeleteTextures(8, mColorAttachmentIDs);
    glDeleteTextures(1, &mDepthAttachmentID);
    glDeleteFramebuffers(1, &mRenderID);
}

void OpenGLFramebuffer::Bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, mRenderID);
}

void OpenGLFramebuffer::Unbind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGLFramebuffer::Resize(const FramebufferSpec& spec, bool forceRecreate)
{
    // Dont resize if width and height haven't changed
    if (!forceRecreate && (spec.width == mWidth && spec.height == mHeight))
    {
        return;
    }

    mWidth = spec.width;
    mHeight = spec.height;
    uint32_t numAttachments = spec.attachments;

    // Clear out old framebuffer data
    if (mRenderID)
    {
        glDeleteTextures(numAttachments, mColorAttachmentIDs);
        glDeleteTextures(1, &mDepthAttachmentID);
        glDeleteFramebuffers(1, &mRenderID);
    }

    // Generate new framebuffer
    glGenTextures(numAttachments, mColorAttachmentIDs);
    glCreateFramebuffers(1, &mRenderID);
    glBindFramebuffer(GL_FRAMEBUFFER, mRenderID);

    // Multisample texture for FBO
    for (int i = 0; i < numAttachments; ++i)
    {
        if (spec.samples > 0)
        {
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, mColorAttachmentIDs[i]);
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, spec.samples, spec.internalFormat, mWidth, mHeight, false);
        }
        else
        {
            glBindTexture(GL_TEXTURE_2D, mColorAttachmentIDs[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, spec.internalFormat, mWidth, mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        }
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);


        if (spec.samples > 0)
        {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D_MULTISAMPLE, mColorAttachmentIDs[i], 0);
        }
        else
        {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, mColorAttachmentIDs[i], 0);
        }
    }

    // TODO add option for specifying type of frame buffer
    // Create rbo for depth and stencil
    GLuint rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, mWidth, mHeight);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    // Attach rbo
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        //LOG_ERROR("ERROR::FRAMEBUFFER:: Framebuffer not complete!");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
