#pragma once

#include <lua.hpp>
#include "Scene/Entity.h"
#include "Scene/Component.h"
#include "Scene/SceneManager.h"
#include "Physics/PhysicsManager.h"

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

    bool IsValid()
    {
        return entity.IsValid();
    }

    Entity Instantiate(Entity)
    {
    }

    Entity Instantiate(Entity other, glm::vec3 position)
    {
        Entity newEntity = entity.scene->DuplicateEntity(other);
        newEntity.GetComponent<TransformComponent>().position = position;
        PhysicsManager::SubmitEntity(newEntity);
    }

    Entity Instantiate(const std::string& prefabName, glm::vec3 position)
    {
        if (ResourceManager::mPrefabMap.find(prefabName) != ResourceManager::mPrefabMap.end())
        {
            nlohmann::json eJson = ResourceManager::mPrefabMap.at(prefabName);
            Entity newEntity = SceneManager::DeserializeEntity(eJson, entity.scene);
            newEntity.GetComponent<TransformComponent>().position = position;
            PhysicsManager::SubmitEntity(newEntity);
            return newEntity;
        }
        CORE_WARN("No prefab named {0} is found within Assets", prefabName);
    }

    void Destroy(Entity other, float timer = 0.0f)
    {
        entity.scene->RemoveEntity(other);
    }

    void Destroy(ScriptableEntity* other, float timer = 0.0f)
    {
        entity.scene->RemoveEntity(other->entity);
    }

    Entity GetEntity() { return entity; }

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
