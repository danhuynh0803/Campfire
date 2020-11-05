#include "Renderer/Texture.h"
#include "Renderer/RendererAPI.h"
#include "Platform/OpenGL/OpenGLTexture.h"
#include "Core/ResourceManager.h"

SharedPtr<Texture2D> Texture2D::Create(const std::string& path)
{
    //switch (RendererAPI::GetAPI())
    //{
    //    case RendererAPI::API::None:
    //        return nullptr;
    //    case RendererAPI::API::OpenGL:
    //        //return CreateSharedPtr<OpenGLTexture2D>(path);
    //        return ResourceManager::GetTexture2D(path);
    //}

    return ResourceManager::GetTexture2D(path);
}

SharedPtr<Texture2D> Texture2D::Create(uint32_t width, uint32_t height)
{
    switch (RendererAPI::GetAPI())
    {
        case RendererAPI::API::None:
            return nullptr;
        case RendererAPI::API::OpenGL:
            return CreateSharedPtr<OpenGLTexture2D>(width, height);
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

SharedPtr<TextureCube> TextureCube::Create(const std::vector<std::string>& pathList)
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
