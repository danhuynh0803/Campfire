#include "Renderer/RendererAPI.h"
#include "OpenGL/OpenGLRendererAPI.h"

RendererAPI::API RendererAPI::sAPI = RendererAPI::API::OpenGL;

UniquePtr<RendererAPI> RendererAPI::Create()
{
    switch (RendererAPI::GetAPI())
    {
        case RendererAPI::API::None:
            return nullptr;
        case RendererAPI::API::OpenGL:
            return CreateUniquePtr<OpenGLRendererAPI>();
    }

    return nullptr;
}
