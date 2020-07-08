#include "Renderer/Texture.h"
#include "Renderer/RendererAPI.h"
#include "Platform/OpenGL/OpenGLTexture.h"

SharedPtr<Texture2D> Texture2D::Create(const std::string& path)
{
    switch (RendererAPI::GetAPI())
    {
        case RendererAPI::API::None:
            return nullptr;
        case RendererAPI::API::OpenGL:
            return CreateSharedPtr<OpenGLTexture2D>(path);
    }

    return nullptr;
}

//SharedPtr<Texture3D> Texture3D::Create(const std::string& path)
//{
//    switch (RendererAPI::GetAPI())
//    {
//        case RendererAPI::API::None:
//            return nullptr;
//        case RendererAPI::API::OpenGL:
//            return CreateSharedPtr<OpenGLTexture3D>(path);
//    }
//    return nullptr;
//}

SharedPtr<TextureCube> Create(const std::vector<std::string>& pathList)
{
    switch (RendererAPI::GetAPI())
    {
        case RendererAPI::API::None:
            return nullptr;
        case RendererAPI::API::OpenGL:
            return CreateSharedPtr<OpenGLTextureCube>(pathList);
    }

    return nullptr;
}
