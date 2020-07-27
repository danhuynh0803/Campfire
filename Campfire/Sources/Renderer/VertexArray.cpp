#include "Renderer/RendererAPI.h"
#include "Renderer/VertexArray.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"

#include <memory>

std::shared_ptr<VertexArray> VertexArray::Create()
{
    switch (RendererAPI::GetAPI())
    {
        case RendererAPI::API::None:
            return nullptr;
        case RendererAPI::API::OpenGL:
            return std::make_shared<OpenGLVertexArray>();
    }

    return nullptr;
}
