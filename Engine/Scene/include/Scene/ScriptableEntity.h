#pragma once

#include "Scene/Entity.h"
#include "Scene/Component.h"
#include <lua.hpp>

class ScriptableEntity
{
public:
    virtual ~ScriptableEntity() {}

    template <typename T>
    T& GetComponent()
    {
        //if (entity.HasComponent<)
        return entity.GetComponent<T>();
    }

    template <typename T>
    bool HasComponent()
    {
        return entity.HasComponent<T>();
    }

    Entity Instantiate(Entity)
    {
    }

    Entity Instantiate(Entity, glm::vec3 position)
    {
        Entity newEntity = entity.scene->CreateEntity("InstObject");
        newEntity.GetComponent<TransformComponent>().position = position;
        //newEntity.AddComponent<MeshComponent>(MeshComponent::Geometry::CUBE);

        return newEntity;
    }

    void Destroy(Entity other, float timer = 0.0f)
    {
        entity.scene->RemoveEntity(other);
    }

    void Destroy(ScriptableEntity* other, float timer = 0.0f)
    {
        entity.scene->RemoveEntity(other->entity);
    }

protected:
    virtual void Start() {}
    virtual void Destroy() {}
    virtual void Update(float dt) {}
    virtual void OnTriggerEnter(Entity other) {}
    virtual void OnTriggerStay(Entity other) {}
    virtual void OnTriggerExit(Entity other) {}
    Entity entity;
    std::string filepath;

private:
    friend class Scene;
};
