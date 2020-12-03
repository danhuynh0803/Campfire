#pragma once

#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <glm/glm.hpp>
#include "Physics/Collider.h"

#include <entt.hpp>

class Trigger
{
public:
    void Construct(const glm::vec3& pos, const glm::vec3& euler, const glm::vec3& scale);
    void Construct(const glm::vec3& pos, const glm::vec3& euler, const glm::vec3& scale, btCollisionShape* shape);

    btGhostObject* GetBulletGhostObject() { return ghostObject; }
    SharedPtr<Collider> collider = Collider::Create(Collider::Shape::BOX);

    void RemoveEntityHandle(entt::entity);
    std::vector<entt::entity> prevList;
    std::vector<entt::entity> overlapEnterList;
    std::vector<entt::entity> overlapExitList;

private:

    btGhostObject* ghostObject = nullptr;
};
