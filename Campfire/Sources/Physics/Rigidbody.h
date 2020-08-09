#ifndef RIGIDBODY_H
#define RIGIDBODY_H

#include <btBulletDynamicsCommon.h>
#include "Core/Base.h"
#include "Physics/Collider.h"

class Rigidbody
{
public:
    void Construct(const glm::vec3& pos, const glm::vec3& euler, const SharedPtr<Collider>& collider)
    {
        btTransform transform;
        transform.setIdentity();
        transform.setOrigin(btVector3(pos.x, pos.y, pos.z));
        btQuaternion quat(glm::radians(euler.x), glm::radians(euler.y), glm::radians(euler.z));
        transform.setRotation(quat);
        btVector3 localInertia(angularDrag, angularDrag, angularDrag);

        btCollisionShape* shape = nullptr;
        if (collider != nullptr)
        {
            btCollisionShape* shape = collider->shape;
        }
        else
        {
            shape = new btBoxShape(btVector3(1.0f, 1.0f, 1.0f));
        }

        if (isDynamic)
        {
            shape->calculateLocalInertia(mass, localInertia);
        }

        btDefaultMotionState* motionState = new btDefaultMotionState(transform);
        btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape, localInertia);
        bulletRigidbody = new btRigidBody(rbInfo);
    }

    btRigidBody* GetBulletRigidbody()
    {
        return bulletRigidbody;
    }

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
