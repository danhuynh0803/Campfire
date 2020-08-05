#include "Scene/Scene.h"
#include "Core/Random.h"
#include <imgui.h>

#include "Renderer/Renderer.h"

Scene::Scene()
{
    /*
       Scene should be initialized with the following:
       1) Camera
       2) Directional light
    */

    auto mainCamera = CreateEntity("camera");
}

void Scene::Init()
{

}

void Scene::OnUpdate(float dt)
{

}

void Scene::OnRenderEditor(float dt, const Camera& editorCamera)
{

    // Only render objects that have mesh components
    auto group = registry.group<MeshComponent>(entt::get<TransformComponent>);
    for (auto entity : group)
    {
        auto [transformComponent, meshComponent] = group.get<TransformComponent, MeshComponent>(entity);
        if (meshComponent.mesh)
        {
            meshComponent.mesh->OnUpdate(dt);

            Renderer::SubmitMesh(meshComponent, transformComponent);
        }
    }

}

void Scene::OnRenderRuntime(float dt)
{
    // Render scene from perspective of game camera

    // Only render objects that have mesh components
    auto group = registry.group<MeshComponent>(entt::get<TransformComponent>);
    for (auto entity : group)
    {
        auto [transformComponent, meshComponent] = group.get<TransformComponent, MeshComponent>(entity);
        if (meshComponent.mesh)
        {
            meshComponent.mesh->OnUpdate(dt);

            Renderer::SubmitMesh(meshComponent, transformComponent);
        }
    }
}

void Scene::OnEvent(Event& e)
{

}

void Scene::OnImGuiRender()
{
}

void Scene::SetSkybox(SharedPtr<TextureCube> skyboxTex)
{
    //skybox->Load()
}

Entity Scene::CreateEntity(const std::string& name)
{
    auto entity = Entity(registry.create(), this);

    // Random ID for access in hashmap
    auto ID = Random::UINT64T();

    // Default components all entities should have
    entity.AddComponent<IDComponent>(ID);
    entity.AddComponent<TagComponent>(name);
    entity.AddComponent<TransformComponent>();

    entityMap[ID] = entity;

    return entity;
}

void Scene::RemoveEntity(entt::entity entity)
{
    registry.destroy(entity);
}


