#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <memory>
#include <string>

#include "ShaderController.h"
//#include "TextureManager.h"

class ResourceManager
{
public:
    static void Start();
    // Shaders

    // Textures

    // Etc
    inline static std::string GetAssetPath() { return assetPath; }

private:
    static std::string assetPath;
};

#define ASSETS()  ResourceManager::GetAssetPath()

#endif // RESOURCE_MANAGER_H

