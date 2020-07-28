#include <glad/glad.h>
#include "Core/Log.h"

#include "Platform/OpenGL/OpenGLFramebuffer.h"

OpenGLFramebuffer::OpenGLFramebuffer(uint32_t width, uint32_t height, uint32_t samples)
{
    // TODO make resizing
    glGenTextures(1, &colorAttachmentID);

    glCreateFramebuffers(1, &renderID);
    glBindFramebuffer(GL_FRAMEBUFFER, renderID);

    // Multisample texture for FBO
    if (samples > 0)
    {
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, colorAttachmentID);
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGBA8, width, height, false);
    }
    else
    {
        glBindTexture(GL_TEXTURE_2D, colorAttachmentID);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);


    if (samples > 0)
    {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, colorAttachmentID, 0);
    }
    else
    {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorAttachmentID, 0);
    }

    // TODO add option for specifying type of frame buffer
    //// Create rbo for depth and stencil
    //GLuint rbo;
    //glGenRenderbuffers(1, &rbo);
    //glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    //glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    //glBindRenderbuffer(GL_RENDERBUFFER, 0);
    //// Attach rbo
    //glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        LOG_ERROR("ERROR::FRAMEBUFFER:: Framebuffer not complete!");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

OpenGLFramebuffer::~OpenGLFramebuffer()
{
    glDeleteTextures(1, &colorAttachmentID);
    glDeleteTextures(1, &depthAttachmentID);
    glDeleteFramebuffers(1, &renderID);
}

void OpenGLFramebuffer::Bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, renderID);
}

void OpenGLFramebuffer::Unbind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
