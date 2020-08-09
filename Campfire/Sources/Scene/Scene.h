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
    // Render scene based on editor camera
    void OnRenderEditor(float dt, const Camera& editorCamera);
    // Render scene based on game camera
    void OnRenderRuntime(float dt);
    void OnImGuiRender();

    void SetSkybox(SharedPtr<TextureCube> skyboxTex);

    Entity CreateEntity(const std::string& name);
    void RemoveEntity(Entity entity);
    //void RemoveEntityRange();
    //const std::unordered_map<uint64_t, Entity> GetEntityMap() { return entityMap; }
    //std::unordered_map<uint64_t, Entity> GetEntityMap() { return entityMap; }
    std::map<uint64_t, Entity> GetEntityMap() { return entityMap; }

private:
    void SubmitCamera(const Camera& camera);
    void SubmitLights();

private:
    friend class Entity;
    friend class Skybox;

    SharedPtr<UniformBuffer> uboCamera;
    SharedPtr<UniformBuffer> uboLights;
    UniquePtr<Skybox> skybox;

    entt::entity entity;
    entt::registry registry;
    //std::unordered_map<uint64_t, Entity> entityMap;
    std::map<uint64_t, Entity> entityMap;
};

#endif // SCENE_H
