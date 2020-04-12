#ifndef SHARED_H
#define SHARED_H

#include "ObjectManager.h"
#include "ShaderController.h"
#include "SceneLoader.h"
#include "Camera.h"
#include "PhysicsManager.h"

#include <vector>

struct Shared
{
    ObjectManager* objectManager;
    ShaderController* shaderController;
    SceneLoader* sceneLoader;
    PhysicsManager* physicsManager;

    std::vector<Camera> cameras;
};

extern Shared shared;

#endif // SHARED_H
