#include "Core/ResourceManager.h"
#include "Renderer/RendererAPI.h"
#include "OpenGL/OpenGLTexture.h"

std::unordered_map<std::string, SharedPtr<Texture2D>> ResourceManager::mCachedTextureMap;
std::unordered_map<std::string, SharedPtr<Mesh>> ResourceManager::mCachedMeshMap;

std::string ResourceManager::mAssetsPath = "../../Assets/";
std::string ResourceManager::mShaderPath = "../../Shaders/";

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
