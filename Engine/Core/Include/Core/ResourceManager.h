#pragma once

#include <unordered_map>
#include <string>

#include "Base.h"
#include "Renderer/Texture.h"
#include "Renderer/Mesh.h"

class ResourceManager
{
public:
    static SharedPtr<Texture2D> GetTexture2D(const std::string& path);
    static SharedPtr<Mesh> GetMesh(const std::string& path);
    //SharedPtr<Audio> GetAudio(const std::string& path);

    inline static std::string GetAssetsPath() { return mAssetsPath; }
    inline static std::string GetShaderPath() { return mShaderPath; }

private:
    static std::unordered_map<std::string, SharedPtr<Texture2D>> mCachedTextureMap;
    static std::unordered_map<std::string, SharedPtr<Mesh>> mCachedMeshMap;

    static std::string mAssetsPath;
    static std::string mShaderPath;
};

#define ASSETS  ResourceManager::GetAssetsPath()
#define SHADERS ResourceManager::GetShaderPath()
