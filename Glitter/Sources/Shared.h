#ifndef SHARED_H
#define SHARED_H

#include "ObjectManager.h"
#include "ShaderController.h"
#include "SceneLoader.h"

#include <vector>

struct Shared
{
    ObjectManager* objectManager;
    ShaderController* shaderController;
    SceneLoader* sceneLoader;

    std::vector<Camera> cameras;
};

extern Shared shared;

#endif // SHARED_H
