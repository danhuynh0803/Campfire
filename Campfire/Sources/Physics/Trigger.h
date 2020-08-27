#pragma once

#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <glm/glm.hpp>
#include "Physics/Collider.h"

class Trigger
{
public:
    void Construct(const glm::vec3& pos, const glm::vec3& euler, const glm::vec3& scale);

    btGhostObject* GetBulletGhostObject() { return ghostObject; }
    SharedPtr<Collider> collider = Collider::Create(Collider::Shape::BOX);

private:
    btGhostObject* ghostObject = nullptr;
};
