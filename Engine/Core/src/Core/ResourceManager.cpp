#include "Core/ResourceManager.h"
#include "Renderer/RendererAPI.h"
#include "OpenGL/OpenGLTexture.h"
#include "Vulkan/VulkanTexture.h"
#include "Scene/Entity.h"

std::vector<SharedPtr<Texture2D>> ResourceManager::mTextureMaster;
std::unordered_map<std::string, SharedPtr<Texture2D>> ResourceManager::mCachedTextureMap;
std::unordered_map<std::string, SharedPtr<Mesh>> ResourceManager::mCachedMeshMap;
std::unordered_map<std::string, Entity> ResourceManager::mCachedEntityMap;
std::unordered_map<std::string, std::vector<uint32_t>> ResourceManager::mCompiledShaders;
std::unordered_map<std::string, nlohmann::json> ResourceManager::mPrefabMap;

std::string ResourceManager::mAssetsPath = "../../Assets";
std::string ResourceManager::mShaderPath = "../../Shaders";
std::string ResourceManager::mFontsPath = "../../Assets/Fonts";

SharedPtr<Texture2D> ResourceManager::GetTexture2D(const std::string& path)
{
    auto it = mCachedTextureMap.find(path);
    SharedPtr<Texture2D> addedTexture;
    if (it == mCachedTextureMap.end())
    {
        switch (RendererAPI::GetAPI())
        {
            case RendererAPI::API::None:
                return nullptr;
            case RendererAPI::API::OpenGL:
                addedTexture = CreateSharedPtr<OpenGLTexture2D>(path);
                break;
            case RendererAPI::API::Vulkan:
                addedTexture = CreateSharedPtr<VulkanTexture2D>(path);
        }

        mCachedTextureMap.emplace(path, addedTexture);
        mTextureMaster.emplace_back(addedTexture);
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

std::vector<uint32_t> ResourceManager::GetCompiledShader(const std::string& filepath)
{
    // TODO
    return mCompiledShaders.at(filepath);
}
