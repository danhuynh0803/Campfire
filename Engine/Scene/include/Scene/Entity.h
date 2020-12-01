#pragma once

#include <cstdarg>
#include <entt.hpp>
#include <typeinfo>
#include "Core/Base.h"
#include "Scene/Scene.h"

struct RelationshipComponent
{
    std::size_t numChildren = 0;
    entt::entity first{ entt::null };
    entt::entity prev{ entt::null };
    entt::entity next{ entt::null };
    entt::entity parent{ entt::null };
};

class Entity
{
public:
    Entity() = default;
    Entity(entt::entity handle, Scene* _scene);
    ~Entity() = default;

    void AddChild(entt::entity childHandle);

    template <typename T, typename... Args>
    T& AddComponent(Args&&... args)
    {
        return scene->registry.template emplace<T>(entityHandle, std::forward<Args>(args)...);
    }

    template <typename T>
    T& GetComponent()
    {
        if (HasComponent<T>())
            return scene->registry.template get<T>(entityHandle);
        else
            CORE_ERROR("{0} not found", typeid(T).name());
    }

    template <typename T>
    bool HasComponent()
    {
        return scene->registry.template has<T>(entityHandle);
    }

    bool IsValid()
    {
        return scene->registry.valid(entityHandle);
    }

    template <typename T>
    void RemoveComponent()
    {
        if (HasComponent<T>())
            scene->registry.template remove<T>(entityHandle);
        else
            CORE_WARN("{0} not found for removal", typeid(T).name());
    }

    operator bool() const { return entityHandle != entt::null; }
    operator entt::entity() const { return entityHandle; }

private:
    entt::entity entityHandle { entt::null };
    Scene* scene;

    friend class Scene;
    friend class ScriptableEntity;
    friend class Script;
};
