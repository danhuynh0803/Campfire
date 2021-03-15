#include <iostream>
//#include<XX.h>
//RUNTIME_MODIFIABLE_INCLUDE;
#include "RuntimeObjectSystem/IObject.h"
#include "RuntimeObjectSystem/ObjectInterfacePerModule.h"
#include "RuntimeObjectSystem/IObjectFactorySystem.h"
#include "RCCppEntry.h"
#include "RCCppSystemTable.h"

#include "Core/Input.h"
#include "Core/Time.h"
#include "Scene/Entity.h"
#include "Scene/Scene.h"
#include "Renderer/SceneRenderer.h"
#include "Scene/Component.h"

//https://github.com/RuntimeCompiledCPlusPlus/RuntimeCompiledCPlusPlus/wiki/Using-libraries-from-runtime-modifiable-classes
#include "RuntimeObjectSystem/RuntimeLinkLibrary.h"

RUNTIME_COMPILER_LINKLIBRARY("assimp-vc142-mtd.lib");
RUNTIME_COMPILER_LINKLIBRARY("luad.lib");
RUNTIME_COMPILER_LINKLIBRARY("imguid.lib");
RUNTIME_COMPILER_LINKLIBRARY("gladd.lib");
RUNTIME_COMPILER_LINKLIBRARY("Engined.lib");
RUNTIME_COMPILER_LINKLIBRARY("tracyd.lib");
RUNTIME_COMPILER_LINKLIBRARY("glfw3d.lib");
RUNTIME_COMPILER_LINKLIBRARY("fmod_vc.lib");
RUNTIME_COMPILER_LINKLIBRARY("fmodstudio_vc.lib");
RUNTIME_COMPILER_LINKLIBRARY("Bullet2FileLoaderd.lib");
RUNTIME_COMPILER_LINKLIBRARY("Bullet3Collisiond.lib");
RUNTIME_COMPILER_LINKLIBRARY("Bullet3Commond.lib");
RUNTIME_COMPILER_LINKLIBRARY("Bullet3Dynamicsd.lib");
RUNTIME_COMPILER_LINKLIBRARY("Bullet3Geometryd.lib");
RUNTIME_COMPILER_LINKLIBRARY("Bullet3OpenCL_clewd.lib");
RUNTIME_COMPILER_LINKLIBRARY("BulletCollisiond.lib");
RUNTIME_COMPILER_LINKLIBRARY("BulletInverseDynamicsd.lib");
RUNTIME_COMPILER_LINKLIBRARY("BulletSoftBodyd.lib");
RUNTIME_COMPILER_LINKLIBRARY("LinearMathd.lib");

#ifdef _WIN32
RUNTIME_COMPILER_LINKLIBRARY("shell32.lib");//ShellExecuteA
RUNTIME_COMPILER_LINKLIBRARY("Comdlg32.lib");//FileOpenA etc
RUNTIME_COMPILER_LINKLIBRARY("gdi32.lib");//glfw
RUNTIME_COMPILER_LINKLIBRARY("Advapi32.lib");//tracy
#endif
//the complier asked for these source files during the run time for some reason
//https://github.com/RuntimeCompiledCPlusPlus/RuntimeCompiledCPlusPlus/wiki/Runtime-source-dependencies
#include "RuntimeObjectSystem/RuntimeSourceDependency.h"
RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE("../../Vendor/bullet3/src/BulletDynamics/Dynamics/btRigidBody", ".cpp");
RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE("../../Vendor/bullet3/src/BulletDynamics/Dynamics/btDiscreteDynamicsWorld", ".cpp");
RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE("../../Vendor/bullet3/src/BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver", ".cpp");
RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE("../../Vendor/bullet3/src/BulletDynamics/ConstraintSolver/btGeneric6DofConstraint", ".cpp");
RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE("../../Vendor/bullet3/src/BulletDynamics/ConstraintSolver/btTypedConstraint", ".cpp");
RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE("../../Vendor/bullet3/src/BulletDynamics/ConstraintSolver/btConeTwistConstraint", ".cpp");

//RUNTIME_COMPILER_LINKLIBRARY("fmodL_vc.lib");
//RUNTIME_COMPILER_LINKLIBRARY("fmodLibd.lib");
//RUNTIME_COMPILER_LINKLIBRARY("zlibstaticd.lib");

static int currDisplay = 0;
enum InterfaceIDEntryEnum
{
    IID_IRCCPP_ENTRY = IID_ENDInterfaceID
};

//This is the runtime modifiable class
//Ideally I think we would have tons of these, and not just one
//https://github.com/RuntimeCompiledCPlusPlus/RuntimeCompiledCPlusPlus/wiki/Runtime-Modifiable-Classes

struct RCCppEntry : IRCCppEntry, TInterface<IID_IRCCPP_ENTRY, IObject>
{
    SharedPtr<Scene> activeScene = nullptr;
    SharedPtr<Camera> mainGameCamera = nullptr;

    RCCppEntry()
    {
        g_SystemTable->RCCppEntry = this;
        g_SystemTable->runtimeObjectSystem->GetObjectFactorySystem()->SetObjectConstructorHistorySize(10);
        activeScene = CreateSharedPtr<Scene>();
        activeScene->OnStart();
    }

    void Update(float dt) override
    {
        //int ssrea = 1225;
        activeScene->OnUpdate(dt);

        // Setup game camera
        auto group = activeScene->GetAllEntitiesWith<CameraComponent, TransformComponent, RelationshipComponent>();
        for (auto entity : group)
        {
            SharedPtr<Camera> selectedCamera = group.get<CameraComponent>(entity);
            if (selectedCamera->targetDisplay == currDisplay)
            {
                mainGameCamera = selectedCamera;
                auto& transformComponent = group.get<TransformComponent>(entity);

                auto relationshipComponent = group.get<RelationshipComponent>(entity);
                // If entity is a child, then apply parents transform to it
                if (relationshipComponent.parent != entt::null)
                {
                    Entity parent(relationshipComponent.parent, activeScene.get());
                    auto parentTransform = parent.GetComponent<TransformComponent>();

                    glm::mat4 transform = glm::mat4(1.0f);
                    transform = glm::mat4(1.0f);
                    glm::vec3 position = transformComponent.position + parentTransform.position;
                    glm::vec3 eulerAngles = transformComponent.euler;
                    glm::vec3 scale = transformComponent.scale * parentTransform.scale;

                    glm::vec3 parentEulerAngles = parentTransform.euler;
                    glm::quat parentRotation = glm::quat(
                        glm::vec3(
                            glm::radians(parentEulerAngles.x),
                            glm::radians(parentEulerAngles.y),
                            glm::radians(parentEulerAngles.z)
                        )
                    );
                    glm::vec3 rotationPosition = parentTransform.position + (parentRotation * (position - parentTransform.position));

                    mainGameCamera->pos = rotationPosition;
                    mainGameCamera->RecalculateViewMatrix(rotationPosition, transformComponent.euler + parentEulerAngles);
                    mainGameCamera->SetProjection();
                }
                else
                {
                    mainGameCamera->pos = transformComponent.position;
                    mainGameCamera->RecalculateViewMatrix(transformComponent.position, transformComponent.euler);
                    mainGameCamera->SetProjection();
                }
                break;
            }
        }

        if (mainGameCamera)
        {
            SceneRenderer::BeginScene(activeScene, *mainGameCamera);
            activeScene->OnRender(dt, *mainGameCamera);
            SceneRenderer::EndScene();
        }

    }

    void Shutdown() override
    {
    }
};
//REGISTERCLASS(RCCppEntry)
REGISTERSINGLETON(RCCppEntry, true);