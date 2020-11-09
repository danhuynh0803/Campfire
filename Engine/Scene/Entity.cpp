#include "Scene/Entity.h"

Entity::Entity(entt::entity handle, Scene* _scene)
    : entityHandle(handle), scene(_scene)
{
}

void Entity::AddChild(entt::entity childHandle)
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
