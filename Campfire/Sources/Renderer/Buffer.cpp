#include "Buffer.h"

#include "Renderer/RendererAPI.h"
#include "Platform/OpenGL/OpenGLBuffer.h"
#include "Core/Base.h"
#include <stdint.h>

SharedPtr<VertexBuffer> VertexBuffer::Create(uint32_t size)
{
    switch (RendererAPI::GetAPI())
    {
        case RendererAPI::API::None:
            return nullptr;
        case RendererAPI::API::OpenGL:
            return CreateSharedPtr<OpenGLVertexBuffer>(size);
    }

    return nullptr;
}

SharedPtr<VertexBuffer> VertexBuffer::Create(void* vertices, uint32_t size)
{
    switch (RendererAPI::GetAPI())
    {
        case RendererAPI::API::None:
            return nullptr;
        case RendererAPI::API::OpenGL:
            // TODO convert OpenGLBuffer to use void* data so we can pass vertex data directly without converting to float*
            return CreateSharedPtr<OpenGLVertexBuffer>((float*)vertices, size);
    }

    return nullptr;
}

SharedPtr<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t count)
{
    switch (RendererAPI::GetAPI())
    {
        case RendererAPI::API::None:
            return nullptr;
        case RendererAPI::API::OpenGL:
            return CreateSharedPtr<OpenGLIndexBuffer>(indices, count);
    }

    return nullptr;
}

SharedPtr<UniformBuffer> UniformBuffer::Create()
{
    switch (RendererAPI::GetAPI())
    {
        case RendererAPI::API::None:
            return nullptr;
        case RendererAPI::API::OpenGL:
            return CreateSharedPtr<OpenGLUniformBuffer>();
    }

    return nullptr;
}
