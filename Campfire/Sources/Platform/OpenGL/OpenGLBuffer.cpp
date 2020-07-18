#include "OpenGLBuffer.h"

#include <glad/glad.h>
#include "Renderer/Shader.h"
#include "Core/Log.h"

//=====================================================================
//------------------------- Vertex Buffers ----------------------------
//=====================================================================
OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size)
{
    glCreateBuffers(1, &renderID);
    glBindBuffer(GL_ARRAY_BUFFER, renderID);
    glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
}

OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, uint32_t size)
{
    glCreateBuffers(1, &renderID);
    glBindBuffer(GL_ARRAY_BUFFER, renderID);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

OpenGLVertexBuffer::~OpenGLVertexBuffer()
{
    glDeleteBuffers(1, &renderID);
}

void OpenGLVertexBuffer::Bind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, renderID);
}

void OpenGLVertexBuffer::Unbind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void OpenGLVertexBuffer::SetData(void* data, uint32_t size)
{
    glBindBuffer(GL_ARRAY_BUFFER, renderID);
    glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
}

//=====================================================================
//------------------------- Index Buffers -----------------------------
//=====================================================================
OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* indices, uint32_t _count)
    : count(_count)
{
    glCreateBuffers(1, &renderID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, _count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
}

OpenGLIndexBuffer::~OpenGLIndexBuffer()
{
    glDeleteBuffers(1, &renderID);
}

void OpenGLIndexBuffer::Bind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderID);
}

void OpenGLIndexBuffer::Unbind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

uint32_t OpenGLIndexBuffer::GetCount() const
{
    return count;
}

//=====================================================================
//------------------------ Uniform Buffers ----------------------------
//=====================================================================
OpenGLUniformBuffer::OpenGLUniformBuffer()
{
    glCreateBuffers(1, &renderID);
    glBindBuffer(GL_UNIFORM_BUFFER, renderID);
}

OpenGLUniformBuffer::~OpenGLUniformBuffer()
{
    glDeleteBuffers(1, &renderID);
}

void OpenGLUniformBuffer::Bind() const
{
    glBindBuffer(GL_UNIFORM_BUFFER, renderID);
}

void OpenGLUniformBuffer::Unbind() const
{
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void OpenGLUniformBuffer::SetLayout(const BufferLayout& _layout, uint32_t blockIndex)
{
    layout = _layout;

    uint32_t size = 0;
    for (const auto& element : layout)
    {
        size += ConvertShaderDataTypeToSize(element.type);
    }

    // Allocate buffer based on attached elements
    glBindBuffer(GL_UNIFORM_BUFFER, renderID);
    glBufferData(GL_UNIFORM_BUFFER, size, NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferRange(GL_UNIFORM_BUFFER, blockIndex, renderID, 0, size);
}

//void OpenGLUniformBuffer::SubmitData()
//{
//    int offset = 0;
//    glBindBuffer(GL_UNIFORM_BUFFER, renderID);
//    glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(glm::mat4), glm::value_ptr(view));
//}

//=====================================================================
//------------------------ Frame Buffers ------------------------------
//=====================================================================
OpenGLFrameBuffer::OpenGLFrameBuffer(uint32_t width, uint32_t height, uint32_t samples)
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

OpenGLFrameBuffer::~OpenGLFrameBuffer()
{
    glDeleteTextures(1, &colorAttachmentID);
    glDeleteTextures(1, &depthAttachmentID);
    glDeleteFramebuffers(1, &renderID);
}

void OpenGLFrameBuffer::Bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, renderID);
}

void OpenGLFrameBuffer::Unbind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
