#include "Buffer.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLBuffer.h"
#include "Core/Base.h"
#include <stdint.h>

SharedPtr<VertexBuffer> VertexBuffer::Create(float* vertices, uint32_t size)
{
    switch (Renderer::GetAPI())
    {
        case RendererAPI::None:
            return nullptr;
        case RendererAPI::OpenGL:
            return CreateSharedPtr<OpenGLVertexBuffer>(vertices, size);
    }

    return nullptr;
}

SharedPtr<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t count)
{
    switch (Renderer::GetAPI())
    {
        case RendererAPI::None:
            return nullptr;
        case RendererAPI::OpenGL:
            return CreateSharedPtr<OpenGLIndexBuffer>(indices, count);
    }

    return nullptr;
}
