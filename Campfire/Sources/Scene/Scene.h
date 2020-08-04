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
    void OnUpdate(float timestep);
    void OnEvent(Event& e);
    void OnImGuiRender();

    void SetSkybox(SharedPtr<TextureCube> skyboxTex);

    Entity CreateEntity(const std::string& name);
    void RemoveEntity(entt::entity entity);
    //void RemoveEntityRange();

private:
    std::unordered_map<uint64_t, Entity> entityMap;

    UniquePtr<Skybox> skybox;

    entt::entity entity;
    entt::registry registry;

    friend class Entity;
    friend class Skybox;
};

#endif // SCENE_H
