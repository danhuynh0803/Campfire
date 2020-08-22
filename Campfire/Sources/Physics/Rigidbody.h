#ifndef RIGIDBODY_H
#define RIGIDBODY_H

#include <btBulletDynamicsCommon.h>
#include "Core/Base.h"
#include "Physics/Collider.h"

class Rigidbody
{
public:
    ~Rigidbody()
    {
        //PhysicsManager::RemoveEntity(bulletRigidbody);
    }

    void Construct(const glm::vec3& pos, const glm::vec3& euler, const SharedPtr<Collider>& collider);
    btRigidBody* GetBulletRigidbody();

public:
    float mass = 1.0f;
    float drag = 0.0f;
    float angularDrag = 0.05f;

    bool useGravity = true;
    bool isDynamic = true;
    bool isKinematic = false;

    bool freezePosition[3] { false, false, false };
    bool freezeRotation[3] { false, false, false };

private:
    btRigidBody* bulletRigidbody;
};

#endif // RIGIDBODY_H
