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

    Entity Instantiate(Entity);
    Entity Instantiate(Entity, glm::vec3 position);

    void Destroy(ScriptableEntity* other)
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
