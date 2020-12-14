#include "GameLayer.h"

#include "Core/Input.h"
#include "Core/Time.h"
#include "Scene/Entity.h"
#include "Scene/Scene.h"
#include "Renderer/SceneRenderer.h"
#include "Scene/Component.h"
#include "Renderer/Framebuffer.h"

SharedPtr<Framebuffer> sceneFBO;

// TODO should be a field within the scene file?

static int currDisplay = 0;

GameLayer::GameLayer()
    : Layer("Game")
{
    // TODO pass a parameter so that we load up the specified scene
    activeScene = CreateSharedPtr<Scene>();
}

void GameLayer::OnAttach()
{
    //sceneFBO

    activeScene->OnStart();
}

void GameLayer::OnDetach()
{
}

void GameLayer::OnUpdate(float dt)
{
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
                glm::vec3 position    = transformComponent.position + parentTransform.position;
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
        //gameCamFBO->Bind();
        SceneRenderer::BeginScene(activeScene, *mainGameCamera);
        activeScene->OnRender(dt, *mainGameCamera);
        SceneRenderer::EndScene();
        //gameCamFBO->Unbind();
    }
    else
    {
        // LOG_ERROR("No game camera found in scene");
    }

    // Post process effect

}

void GameLayer::OnImGuiRender()
{
    // TODO
    // HUDs and such
}

void GameLayer::OnEvent(Event& event)
{
    //EventDispatcher dispatcher(event);
}
