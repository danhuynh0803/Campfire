#include "OpenGLVertexArray.h"

#include <glad/glad.h>
#include "Core/Base.h"

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

OpenGLVertexArray::OpenGLVertexArray()
{
    glCreateVertexArrays(1, &renderID);
}

OpenGLVertexArray::~OpenGLVertexArray()
{
    glDeleteVertexArrays(1, &renderID);
}

void OpenGLVertexArray::Bind() const
{
    glBindVertexArray(renderID);
}

void OpenGLVertexArray::Unbind() const
{
    glBindVertexArray(0);
}

void OpenGLVertexArray::AddVertexBuffer(const SharedPtr<VertexBuffer>& vertexBuffer)
{
    glBindVertexArray(renderID);
    vertexBuffer->Bind();

    // Set up glVertrixAttributes
    const auto& layout = vertexBuffer->GetLayout();
    int index = 0;
    for (const auto& element : layout)
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

    vertexBuffers.push_back(vertexBuffer);
}

void OpenGLVertexArray::SetIndexBuffer(const SharedPtr<IndexBuffer>& indexBuffer)
{
    glBindVertexArray(renderID);
    indexBuffer->Bind();

    this->indexBuffer = indexBuffer;
}
