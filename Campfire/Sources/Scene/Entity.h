#pragma once

#include <cstdarg>
#include <entt.hpp>

#include "Core/Base.h"
#include "Scene/Scene.h"
#include "Scene/Component.h"

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

    void AddChild(entt::entity childHandle)
    {
        auto& parentComp = GetComponent<RelationshipComponent>();

        auto curr = parentComp.first;
        entt::entity prev = entt::null;

        // Iterate to the end of the children list
        for (std::size_t i = 0; i < parentComp.numChildren; ++i)
        {
            prev = curr;
            curr = scene->registry.get<RelationshipComponent>(curr).next;
        }

        // Add new child to end of the list
        curr = childHandle;
        auto& newChildRelation = scene->registry.get<RelationshipComponent>(curr);
        newChildRelation.parent = entityHandle;
        newChildRelation.prev = prev;

        // Child is first in the list
        // Set up link from parent to first child
        if (prev == entt::null)
        {
            parentComp.first = curr;
        }
        // else, child is not the first in the list,
        // then set up link to it from previous child
        else
        {
            auto& prevChildRelation = scene->registry.get<RelationshipComponent>(prev);
            prevChildRelation.next = curr;
        }

        parentComp.numChildren++;
    }

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
            LOG_ERROR("Component not found");
    }

    template <typename T>
    bool HasComponent()
    {
        return scene->registry.template has<T>(entityHandle);
    }

    template <typename T>
    void RemoveComponent()
    {
        scene->registry.template remove<T>(entityHandle);
    }

    operator bool() const { return entityHandle != entt::null; }
    operator entt::entity() const { return entityHandle; }

private:
    friend class Scene;
    friend class ScriptableEntity;

    entt::entity entityHandle { entt::null };

    // Scene that this entity belongs to
    Scene* scene = nullptr;
};
