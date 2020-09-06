#include "Scene/Entity.h"

Entity::Entity(entt::entity handle, Scene* _scene)
    : entityHandle(handle), scene(_scene)
{
}
