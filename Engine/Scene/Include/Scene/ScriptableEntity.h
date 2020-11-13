#pragma once

#include "Scene/Entity.h"

class ScriptableEntity
{
public:
    virtual ~ScriptableEntity() {}

    template <typename T>
    T& GetComponent()
    {
        return entity.GetComponent<T>();
    }

    Entity Instantiate(Entity)
    {
    }

    Entity Instantiate(Entity, glm::vec3 position)
    {
        auto newEntity = entity.scene->CreateEntity("InstObject");
        newEntity.GetComponent<TransformComponent>().position = position;
        newEntity.AddComponent<MeshComponent>(MeshComponent::Geometry::CUBE);

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

private:
    Entity entity;
    friend class Scene;
};
