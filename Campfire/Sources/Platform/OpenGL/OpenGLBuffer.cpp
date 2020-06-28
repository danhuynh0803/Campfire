#include "OpenGLBuffer.h"

#include <glad/glad.h>

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

static GLenum ConvertShaderDataTypeToGLenum(ShaderDataType type)
{
    switch (type)
    {
        case ShaderDataType::INT:       return GL_INT;
        case ShaderDataType::INT2:      return GL_INT;
        case ShaderDataType::INT3:      return GL_INT;
        case ShaderDataType::INT4:      return GL_INT;

        case ShaderDataType::FLOAT:     return GL_FLOAT;
        case ShaderDataType::FLOAT2:    return GL_FLOAT;
        case ShaderDataType::FLOAT3:    return GL_FLOAT;
        case ShaderDataType::FLOAT4:    return GL_FLOAT;

        case ShaderDataType::MAT3:      return GL_FLOAT;
        case ShaderDataType::MAT4:      return GL_FLOAT;

        case ShaderDataType::BOOL:      return GL_INT;
    }
}

void OpenGLVertexBuffer::SetLayout(const BufferLayout& _layout)
{
    layout = _layout;

    // Set up glVertrixAttributes
    int index = 0;
    for (const auto& element : layout.GetElements())
    {
        glVertexAttribPointer(
            index,
            element.componentCount,
            ConvertShaderDataTypeToGLenum(element.type),
            element.normalized,
            layout.GetStride(),
            (const void*) element.offset
        );
        glEnableVertexAttribArray(index);

        index++;
    }
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
