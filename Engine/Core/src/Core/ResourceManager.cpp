#include "Core/ResourceManager.h"
#include "Renderer/RendererAPI.h"
#include "OpenGL/OpenGLTexture.h"
#include "Scene/Entity.h"

std::unordered_map<std::string, SharedPtr<Texture2D>> ResourceManager::mCachedTextureMap;
std::unordered_map<std::string, SharedPtr<Mesh>> ResourceManager::mCachedMeshMap;
std::unordered_map<std::string, Entity> ResourceManager::mCachedEntityMap;

std::string ResourceManager::mAssetsPath = "../../Assets/";
std::string ResourceManager::mShaderPath = "../../Shaders/";
std::string ResourceManager::mFontsPath = "../../Assets/Fonts/";

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

Entity ResourceManager::GetEntityWithTag(const std::string& tag)
{
    auto it = mCachedEntityMap.find(tag);
    if (it == mCachedEntityMap.end())
    {
        LOG_WARN("No prefab for {0} is found", tag);
        return Entity {};
    }
    return mCachedEntityMap.at(tag);
}
