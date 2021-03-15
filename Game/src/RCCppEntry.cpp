#include <iostream>
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

#include "RuntimeObjectSystem/RuntimeLinkLibrary.h"

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
RUNTIME_COMPILER_LINKLIBRARY("assimp-vc142-mtd.lib");

//Win32 libs
RUNTIME_COMPILER_LINKLIBRARY("shell32.lib");//ShellExecuteA
RUNTIME_COMPILER_LINKLIBRARY("Comdlg32.lib");//FileOpenA
RUNTIME_COMPILER_LINKLIBRARY("gdi32.lib");//glfw
RUNTIME_COMPILER_LINKLIBRARY("Advapi32.lib");//tracy

//the complier asked for these source files during the run time for some reason
#include "RuntimeObjectSystem/RuntimeSourceDependency.h"
RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE("../../Vendor/bullet3/src/BulletDynamics/Dynamics/btRigidBody", ".cpp");
RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE("../../Vendor/bullet3/src/BulletDynamics/Dynamics/btDiscreteDynamicsWorld", ".cpp");
RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE("../../Vendor/bullet3/src/BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver", ".cpp");
RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE("../../Vendor/bullet3/src/BulletDynamics/ConstraintSolver/btGeneric6DofConstraint", ".cpp");
RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE("../../Vendor/bullet3/src/BulletDynamics/ConstraintSolver/btTypedConstraint", ".cpp");
RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE("../../Vendor/bullet3/src/BulletDynamics/ConstraintSolver/btConeTwistConstraint", ".cpp");
//For Windows
//RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE("../../Engine/Platform/Windows/src/Windows/WindowsFileSystem", ".cpp");

//RUNTIME_COMPILER_LINKLIBRARY("fmodL_vc.lib");
//RUNTIME_COMPILER_LINKLIBRARY("fmodLibd.lib");
//RUNTIME_COMPILER_LINKLIBRARY("zlibstaticd.lib");
//RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE("../../Vendor/glfw/src/window", ".c"); //
//RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE("../../Vendor/glfw/src/init", ".c");
//RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE("../../Engine/Core/src/Core/Application", ".cpp");
//RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE("../../Engine/Core/src/Core/FileSystem", ".cpp");
//RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE("../../Engine/Core/src/Core/Input", ".cpp");
//RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE("../../Engine/Core/src/Core/Log", ".cpp");
//RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE("../../Engine/Core/src/Core/LogWidget", ".cpp");
//RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE("../../Engine/Core/src/Core/ResourceManager", ".cpp");
//RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE("../../Engine/Scene/src/Scene", ".cpp");
//RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE("../../Engine/Scene/src/Camera", ".cpp");
//RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE("../../Engine/Scene/src/Entity", ".cpp");
//RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE("../../Engine/Scene/src/SceneManager", ".cpp");
//RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE("../../Engine/Scene/src/Skybox", ".cpp");
//RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE("../../Engine/Renderer/src/Renderer/Buffer", ".cpp");
//RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE("../../Engine/Renderer/src/Renderer/Material", ".cpp");
//RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE("../../Engine/Renderer/src/Renderer/Mesh", ".cpp");
//RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE("../../Engine/Renderer/src/Renderer/Renderer", ".cpp");
//RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE("../../Engine/Renderer/src/Renderer/Renderer2D", ".cpp");
//RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE("../../Engine/Renderer/src/Renderer/RendererAPI", ".cpp");
//RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE("../../Engine/Renderer/src/Renderer/RenderCommand", ".cpp");
//RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE("../../Engine/Renderer/src/Renderer/SceneRenderer", ".cpp");
//RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE("../../Engine/Renderer/src/Renderer/Shader", ".cpp");
//RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE("../../Engine/Renderer/src/Renderer/Texture", ".cpp");
//RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE("../../Engine/Renderer/src/Renderer/VertexArray", ".cpp");
//RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE("../../Engine/Renderer/OpenGl/src/OpenGL/OpenGLBuffer", ".cpp");
//RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE("../../Engine/Renderer/OpenGl/src/OpenGL/OpenGLRendererAPI", ".cpp");
//RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE("../../Engine/Renderer/OpenGl/src/OpenGL/OpenGLShader", ".cpp");
//RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE("../../Engine/Renderer/OpenGl/src/OpenGL/OpenGLTexture", ".cpp");
//RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE("../../Engine/Renderer/OpenGl/src/OpenGL/OpenGLVertexArray", ".cpp");
//RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE("../../Engine/Scripting/src/LuaScript", ".cpp");
//RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE("../../Engine/Scripting/src/LuaManager", ".cpp");
//RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE("../../Engine/Scripting/src/LuaUtility", ".cpp");
//RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE("../../Engine/Scripting/src/Lua/LuaEntity", ".cpp");
//RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE("../../Engine/Scripting/src/Lua/LuaAudioSource", ".cpp");
//RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE("../../Engine/Scripting/src/Lua/LuaCamera", ".cpp");
//RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE("../../Engine/Scripting/src/Lua/LuaCollider", ".cpp");
//RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE("../../Engine/Scripting/src/Lua/LuaGlobal", ".cpp");
//RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE("../../Engine/Scripting/src/Lua/LuaInput", ".cpp");
//RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE("../../Engine/Scripting/src/Lua/LuaRigidbody", ".cpp");
//RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE("../../Engine/Scripting/src/Lua/LuaTag", ".cpp");
//RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE("../../Engine/Scripting/src/Lua/LuaTransform", ".cpp");
//RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE("../../Engine/Scripting/src/Lua/LuaVector", ".cpp");
//RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE("../../Engine/Particles/src/ParticleSystem", ".cpp");
//RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE("../../Engine/Physics/src/BulletDebugDrawer", ".cpp");
//RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE("../../Engine/Physics/src/Collider", ".cpp");
//RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE("../../Engine/Physics/src/PhysicsManager", ".cpp");
//RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE("../../Engine/Physics/src/Rigidbody", ".cpp");
//RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE("../../Engine/Physics/src/Trigger", ".cpp");
//RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE("../../Engine/Audio/src/AudioSource", ".cpp");
//RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE("../../Engine/Audio/src/AudioSystem", ".cpp");
//RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE("../../Engine/Audio/src/FmodImpl", ".cpp");

static int currDisplay = 0;
enum InterfaceIDEntryEnum
{
    IID_IRCCPP_ENTRY = IID_ENDInterfaceID
};

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
        int ssrea = 0;
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
};

REGISTERSINGLETON(RCCppEntry, true);