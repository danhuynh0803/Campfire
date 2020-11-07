#include "ResourceManager.h"
#include "Renderer/RendererAPI.h"
#include "Platform/OpenGL/OpenGLTexture.h"

std::unordered_map<std::string, SharedPtr<Texture2D>> ResourceManager::mCachedTextureMap;
std::unordered_map<std::string, SharedPtr<Mesh>> ResourceManager::mCachedMeshMap;

SharedPtr<Texture2D> ResourceManager::GetTexture2D(const std::string& path)
{
    auto it = mCachedTextureMap.find(path);
    if (it == mCachedTextureMap.end())
    {
        switch (RendererAPI::GetAPI())
        {
            case RendererAPI::API::None:
                return nullptr;
            case RendererAPI::API::OpenGL:
                //return CreateSharedPtr<OpenGLTexture2D>(path);
                mCachedTextureMap.emplace(path, CreateSharedPtr<OpenGLTexture2D>(path));
                break;
        }
    }
    return mCachedTextureMap.at(path);
}

SharedPtr<Mesh> ResourceManager::GetMesh(const std::string& path)
{
    auto it = mCachedMeshMap.find(path);
    if (it == mCachedMeshMap.end())
    {
        mCachedMeshMap.emplace(path, CreateSharedPtr<Mesh>(path));
    }
    return mCachedMeshMap.at(path);
}
