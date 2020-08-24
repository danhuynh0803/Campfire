#ifndef RIGIDBODY_H
#define RIGIDBODY_H

#include <btBulletDynamicsCommon.h>
#include "Core/Base.h"
#include "Physics/Collider.h"

class Rigidbody
{
public:
    btRigidBody* GetBulletRigidbody();
    void Construct(const glm::vec3& pos, const glm::vec3& euler, const SharedPtr<Collider>& collider);
    void SetVelocity(glm::vec3 newVelocity);
    void AddVelocity(glm::vec3 velocity);

public:
    enum BodyType
    {
        STATIC = 0,
        KINEMATIC,
        DYNAMIC
    };

    BodyType type = BodyType::DYNAMIC;
    float mass = 1.0f;
    float drag = 0.0f;
    float angularDrag = 0.05f;
    bool useGravity = true;
    bool freezePosition[3] { false, false, false };
    bool freezeRotation[3] { false, false, false };

private:
    btRigidBody* bulletRigidbody;
};

#endif // RIGIDBODY_H
