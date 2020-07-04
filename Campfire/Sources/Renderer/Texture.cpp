#include "Texture.h"
#include "Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLTexture.h"

SharedPtr<Texture> Texture::Create(const std::string& path)
{
    switch (Renderer::GetAPI())
    {
        case RendererAPI::None:
            return nullptr;
        case RendererAPI::OpenGL:
            return CreateSharedPtr<OpenGLTexture>(path);
    }

    return nullptr;
}
