#ifndef SCENE_H
#define SCENE_H

#include <unordered_map>
#include <entt.hpp>

#include "Core/Base.h"
#include "Renderer/Camera.h"
#include "Renderer/Texture.h"

#include "Scene/Entity.h"
#include "Scene/Skybox.h"

class Entity;

class Scene
{
public:
    Scene();
    void Init();
    void OnEvent(Event& e);
    void OnUpdate(float dt);
    // Render scene based on editor camera
    void OnRenderEditor(float dt, const Camera& editorCamera);
    // Render scene based on game camera
    void OnRenderRuntime(float dt);
    void OnImGuiRender();

    void SubmitLights();

    void SetSkybox(SharedPtr<TextureCube> skyboxTex);

    Entity CreateEntity(const std::string& name);
    void RemoveEntity(entt::entity entity);
    //void RemoveEntityRange();
    const std::unordered_map<uint64_t, Entity> const GetEntityMap() { return entityMap; }

private:
    // UBO for lights
    SharedPtr<UniformBuffer> uboLights;

    // Skybox
    UniquePtr<Skybox> skybox;

    // Entities
    entt::entity entity;
    entt::registry registry;
    std::unordered_map<uint64_t, Entity> entityMap;

    friend class Entity;
    friend class Skybox;
};

#endif // SCENE_H
