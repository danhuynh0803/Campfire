#include <glad/glad.h>
#include "OpenGL/OpenGLBuffer.h"
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

void OpenGLUniformBuffer::SetLayout(const BufferLayout& _layout, uint32_t blockIndex, uint32_t count)
{
    layout = _layout;

    uint32_t size = 0;
    for (const auto& element : layout)
    {
        size += ConvertShaderDataTypeToSize(element.type);
    }
    // For allocating an array of data
    // TODO replace with SSBO at some point
    size *= count;

    // Allocate buffer based on attached elements
    glBindBuffer(GL_UNIFORM_BUFFER, renderID);
    glBufferData(GL_UNIFORM_BUFFER, size, NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferRange(GL_UNIFORM_BUFFER, blockIndex, renderID, 0, size);
}


void OpenGLUniformBuffer::SetData(void* data, uint32_t offset, uint32_t size)
{
    glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
}
