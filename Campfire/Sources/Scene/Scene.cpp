#include "Scene/Scene.h"

void Scene::Init()
{

}

void Scene::OnUpdate(float timestep)
{

}

void Scene::OnEvent(Event& e)
{

}

void Scene::SetSkybox(SharedPtr<TextureCube> skyboxTex)
{

}

Entity Scene::CreateEntity(const std::string& name)
{
    auto entity = Entity(registry.create(), this);

    return entity;
}

void Scene::RemoveEntity(entt::entity entity)
{
    registry.destroy(entity);
}


