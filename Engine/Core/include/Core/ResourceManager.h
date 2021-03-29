#pragma once

#include <unordered_map>
#include <string>

#include "Base.h"
#include "Renderer/Texture.h"
#include "Renderer/Mesh.h"
#include <nlohmann/json.hpp>

class Entity;

class ResourceManager
{
public:
    static SharedPtr<Texture2D> GetTexture2D(const std::string& path);
    static SharedPtr<Mesh> GetMesh(const std::string& path);
    static Entity GetEntityWithTag(const std::string& tag);
    static void SetEntityWithTag(Entity entity, const std::string& tag);
    //SharedPtr<Audio> GetAudio(const std::string& path);

    inline static std::string GetAssetsPath() { return mAssetsPath; }
    inline static std::string GetShaderPath() { return mShaderPath; }
    inline static std::string GetFontsPath() { return mFontsPath; }

    static std::unordered_map<std::string, nlohmann::json> mPrefabMap;
    inline static SharedPtr<Texture2D> GetTexture(uint32_t index) { return mTextureMaster.at(index); }
    inline static std::vector<SharedPtr<Texture2D>> GetTextureMaster() { return mTextureMaster; }

private:
    static std::vector<SharedPtr<Texture2D>> mTextureMaster;
    static std::unordered_map<std::string, SharedPtr<Texture2D>> mCachedTextureMap;
    static std::unordered_map<std::string, SharedPtr<Mesh>> mCachedMeshMap;
    static std::unordered_map<std::string, Entity> mCachedEntityMap;

    static std::string mAssetsPath;
    static std::string mShaderPath;
    static std::string mFontsPath;
};

#define ASSETS  ResourceManager::GetAssetsPath()
#define SHADERS ResourceManager::GetShaderPath()
#define FONTS ResourceManager::GetFontsPath()
