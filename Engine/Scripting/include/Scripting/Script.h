#pragma once

#include <lua.hpp>

class Script
{
public:
    Script(Entity e)
        : entity(e)
    {
    }

    template <typename T>
    T& GetComponent()
    {
        return entity.GetComponent<T>();
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

    //void Destroy(ScriptableEntity* other, float timer = 0.0f)
    //{
    //    entity.scene->RemoveEntity(other->entity);
    //}

    virtual void Start() {}
    virtual void Update() {}
    virtual void Update() {}
    virtual void Update(float dt) {}
    virtual void OnTriggerEnter(Entity other) {}
    virtual void OnTriggerStay(Entity other) {}
    virtual void OnTriggerExit(Entity other) {}

private:
    Entity entity;
    std::string filepath;
};
