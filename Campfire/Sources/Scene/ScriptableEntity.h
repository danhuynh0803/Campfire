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

    //Entity Instantiate(Entity);
    //Entity Instantiate(Entity, glm::vec3 position);

protected:
    virtual void Start() {}
    virtual void Destroy() {}
    virtual void Update(float dt) {}
    //virtual void OnTriggerEnter(Collider) {}

private:
    Entity entity;
    friend class Scene;
};
