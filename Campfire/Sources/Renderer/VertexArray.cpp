#include "Renderer/Renderer.h"
#include "Renderer/VertexArray.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"

#include <memory>

std::shared_ptr<VertexArray> VertexArray::Create()
{
    switch (Renderer::GetAPI())
    {
        case RendererAPI::None:
            return nullptr;
        case RendererAPI::OpenGL:
            return std::make_shared<OpenGLVertexArray>();
    }

    return nullptr;
}
