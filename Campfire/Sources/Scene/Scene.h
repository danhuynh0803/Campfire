#ifndef SCENE_H
#define SCENE_H

//#include <unordered_map>
#include <map>
#include <utility>
#include <entt.hpp>

#include "Core/Base.h"
#include "Scene/Camera.h"
#include "Renderer/Texture.h"

#include "Scene/Entity.h"
#include "Scene/Skybox.h"

class Entity;

class Scene
{
public:
    Scene(bool isNewScene = true);
    void Init();
    void OnEvent(Event& e);
    void OnUpdate(float dt);

    void OnRender(float dt);
    // TODO refactor to be one render call with
    // diff view and play state params?
    void OnRenderEditor(float dt, const Camera& editorCamera, bool isPlaying = false);
    void OnRenderRuntime(float dt);
    void OnImGuiRender();

    void Clear() { registry.clear(); }

    SharedPtr<Skybox> GetSkybox() { return skybox; }
    void SetSkybox(SharedPtr<TextureCube> skyboxTex);
    void SetSkybox(SharedPtr<Skybox> newSkybox);

    Entity CreateEntity(const std::string& name);
    Entity CreateEntity(const std::string& name, uint64_t ID);
    void RemoveEntity(Entity entity);
    //void RemoveEntityRange();
    //const std::unordered_map<uint64_t, Entity> GetEntityMap() { return entityMap; }
    //std::unordered_map<uint64_t, Entity> GetEntityMap() { return entityMap; }

    const std::map<uint64_t, Entity> GetEntityMap() { return entityMap; }

    template <typename T>
    auto GetAllEntitiesWith() { return registry.view<T>(); }

private:
    void SubmitCamera(const Camera& camera);
    void SubmitLights();

private:
    friend class Entity;
    friend class Skybox;

    SharedPtr<UniformBuffer> uboCamera;
    SharedPtr<UniformBuffer> uboLights;
    SharedPtr<Skybox> skybox;

    entt::entity entity;
    entt::registry registry;
    //std::unordered_map<uint64_t, Entity> entityMap;
    std::map<uint64_t, Entity> entityMap;
};

#endif // SCENE_H
