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
    void Init();
    void OnUpdate(float timestep);
    void OnEvent(Event& e);

    void SetSkybox(SharedPtr<TextureCube> skyboxTex);

    Entity CreateEntity(const std::string& name);
    void RemoveEntity(entt::entity entity);
    //void RemoveEntityRange();

private:
    entt::entity entity;
    entt::registry registry;

    UniquePtr<Skybox> skybox;

    friend class Entity;
    friend class Skybox;

    //std::unordered_map<uint64_t, Entity> entityMap;
};

#endif // SCENE_H
