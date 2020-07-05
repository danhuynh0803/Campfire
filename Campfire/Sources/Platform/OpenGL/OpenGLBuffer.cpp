#include "OpenGLBuffer.h"

#include <glad/glad.h>
#include "Renderer/Shader.h"

//=====================================================================
//------------------------- Vertex Buffers ----------------------------
//=====================================================================
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
    //glBufferData(GL_UNIFORM_BUFFER, size, NULL, GL_STATIC_DRAW);
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
    glBindBufferBase(GL_UNIFORM_BUFFER, blockIndex, renderID);

    // TODO check BufferRange vs base
    //if (layout.size() > 1)
    //{
    //    glBindBufferRange(GL_UNIFORM_BUFFER, blockIndex, renderID, 0, size);
    //}
    //else
    //{
    //    glBindBufferBase(GL_UNIFORM_BUFFER, blockIndex, renderID);
    //}
}

//void OpenGLUniformBuffer::SubmitData()
//{
//    glBindBuffer(GL_UNIFORM_BUFFER, renderID);
//    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(view));
//}
