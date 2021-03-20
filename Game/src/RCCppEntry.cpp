#include <iostream>

//#include<XX.h>
//RUNTIME_MODIFIABLE_INCLUDE;
#include "RuntimeObjectSystem/ObjectInterfacePerModule.h"
#include "RuntimeObjectSystem/IRuntimeObjectSystem.h"
#include "RCCppEntry.h"
#include "RCCppSystemTable.h"
#include "RCCppEngine.h"
#include "Core/Input.h"
#include "Core/Time.h"
#include "Scene/Entity.h"
#include "Scene/Scene.h"
#include "Renderer/SceneRenderer.h"
#include "Scene/Component.h"

//the complier asked for these source files during the run time for some reason
//https://github.com/RuntimeCompiledCPlusPlus/RuntimeCompiledCPlusPlus/wiki/Runtime-source-dependencies
#include "RuntimeObjectSystem/RuntimeSourceDependency.h"
RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE("../../Vendor/bullet3/src/BulletDynamics/Dynamics/btRigidBody", ".cpp");
RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE("../../Vendor/bullet3/src/BulletDynamics/Dynamics/btDiscreteDynamicsWorld", ".cpp");
RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE("../../Vendor/bullet3/src/BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver", ".cpp");
RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE("../../Vendor/bullet3/src/BulletDynamics/ConstraintSolver/btGeneric6DofConstraint", ".cpp");
RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE("../../Vendor/bullet3/src/BulletDynamics/ConstraintSolver/btTypedConstraint", ".cpp");
RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE("../../Vendor/bullet3/src/BulletDynamics/ConstraintSolver/btConeTwistConstraint", ".cpp");

static int currDisplay = 0;

//This is the runtime modifiable class
//Ideally I think we would have tons of these, and not just one
//https://github.com/RuntimeCompiledCPlusPlus/RuntimeCompiledCPlusPlus/wiki/Runtime-Modifiable-Classes

struct RCCppEntry : IRCCppEntry
{
    SharedPtr<Scene> activeScene = nullptr;
    SharedPtr<Camera> mainGameCamera = nullptr;
    SharedPtr<SceneRenderer> sceneRenderer = nullptr;

    RCCppEntry()
    {
        PerModuleInterface::g_pSystemTable->RCCppEntry = this;
        activeScene = PerModuleInterface::g_pSystemTable->activeScene;
        sceneRenderer = PerModuleInterface::g_pSystemTable->sceneRenderer;
    }

    //virtual Update(float dt)
    //{

    //}

    void Update(float dt) override
    {
        //for (auto it = PerModuleInterface::g_pSystemTable->tests.begin(); it!= PerModuleInterface::g_pSystemTable->tests.end(); ++it)
        //{
        //    (*it)->OnUpdate(dt);
        //}
        //activeScene->OnUpdate(dt);

        //// Setup game camera
        //auto group = activeScene->GetAllEntitiesWith<CameraComponent, TransformComponent, RelationshipComponent>();
        //for (auto entity : group)
        //{
        //    SharedPtr<Camera> selectedCamera = group.get<CameraComponent>(entity);
        //    if (selectedCamera->targetDisplay == currDisplay)
        //    {
        //        mainGameCamera = selectedCamera;
        //        auto& transformComponent = group.get<TransformComponent>(entity);

        //        mainGameCamera->pos = transformComponent.position;
        //        mainGameCamera->RecalculateViewMatrix(transformComponent.position, transformComponent.euler);
        //        mainGameCamera->SetProjection();

        //        break;
        //    }
        //}
        //if (mainGameCamera)
        //{
        //    sceneRenderer->BeginScene(activeScene, *mainGameCamera);
        //    activeScene->OnRender(dt, *mainGameCamera);
        //    sceneRenderer->EndScene();
        //}
    }

    void Shutdown() override
    {
        activeScene->OnStop();
        activeScene.reset();
        mainGameCamera.reset();
        sceneRenderer.reset();
        //delete activeScene;
    }
};
//REGISTERCLASS(RCCppEntry)
//set to true for automatically set to true to have the class automatically constructed and initialized 
REGISTERCLASS(RCCppEntry)
//REGISTERSINGLETON(RCCppEntry, true);