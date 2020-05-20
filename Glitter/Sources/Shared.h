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
    ObjectManager* objectManager = nullptr;
    ShaderController* shaderController = nullptr;
    SceneLoader* sceneLoader = nullptr;
    PhysicsManager* physicsManager = nullptr;

    // The camera used to display scene view
    // and not game view
    Camera* renderCamera = nullptr;
};

extern Shared shared;

#endif // SHARED_H
