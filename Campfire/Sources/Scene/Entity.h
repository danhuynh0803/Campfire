#ifndef ENTITY_H
#define ENTITY_H

#include <cstdarg>
#include <entt.hpp>

#include "Scene/Scene.h"
#include "Scene/Component.h"
#include "Core/Base.h"

class Scene;

class Entity
{
public:
    Entity() = default;
    Entity(entt::entity handle, Scene* _scene)
        : entityHandle(handle), scene(_scene)
    {
    }

    ~Entity() = default;

    template <typename T, typename... Args>
    T& AddComponent(Args&&... args)
    {
        return scene->registry.template emplace<T>(entityHandle, std::forward<Args>(args)...);
    }

    template <typename T>
    void RemoveComponent()
    {
        scene->registry.template remove<T>(entityHandle);
    }

    template <typename T>
    T& GetComponent()
    {
        return scene->registry.template get<T>(entityHandle);
    }

    template <typename T>
    bool HasComponent()
    {
        return scene->registry.template has<T>(entityHandle);
    }

    entt::entity GetHandle() { return entityHandle; }

private:
    entt::entity entityHandle;

    // Scene that this entity belongs to
    Scene* scene = nullptr;

    friend class Scene;
};

#endif // ENTITY_H
