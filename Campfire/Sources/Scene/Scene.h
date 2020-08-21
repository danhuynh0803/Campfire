#ifndef SCENE_H
#define SCENE_H

#include <map>
#include <utility>
#include <entt.hpp>

#include "Core/Base.h"
#include "Scene/Camera.h"
#include "Scene/Skybox.h"
#include "Renderer/SceneRenderer.h"

class Entity;

class Scene
{
public:
    Scene(bool isNewScene = true);
    void Init();
    void OnEvent(Event& e);

    void OnUpdate(float dt);
    void OnRender(float dt, const Camera& camera, bool isPlaying = false);

    template <typename T, typename... Args>
    auto GetAllEntitiesWith() { return registry.view<T, Args...>(); }

    //auto GetAllEntitiesWithTag(const std::string& tag) { return registry.view<T>(); }
    Entity CreateEntity(const std::string& name);
    Entity CreateEntity(const std::string& name, uint64_t ID);
    Entity DuplicateEntity(const Entity& entity);
    void RemoveEntity(Entity entity);
    void Clear() { registry.clear(); }

    const std::map<uint64_t, Entity> GetEntityMap() { return entityMap; }

public:
    SharedPtr<Skybox> skybox;
private:
    friend class Entity;
    friend class Skybox;
    friend class SceneRenderer;

    entt::entity sceneEntity;
    entt::registry registry;
    std::map<uint64_t, Entity> entityMap;
};

#endif // SCENE_H
