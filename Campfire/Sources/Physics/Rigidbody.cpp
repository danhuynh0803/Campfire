#include "Physics/Rigidbody.h"
#include "Physics/PhysicsManager.h"

void Rigidbody::Construct(const glm::vec3& pos, const glm::vec3& euler, const SharedPtr<Collider>& collider)
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
        shape = collider->shape;
    }
    else
    {
        // No collider present, but we create a shape of size 0, in order for physics to still be applied
        shape = new btBoxShape(btVector3(0, 0, 0));
    }

    if (isDynamic)
    {
        shape->calculateLocalInertia(mass, localInertia);
    }

    btDefaultMotionState* motionState = new btDefaultMotionState(transform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape, localInertia);
    bulletRigidbody = new btRigidBody(rbInfo);

    bulletRigidbody->setLinearFactor(
        btVector3(
            static_cast<int>(freezePosition[0]) ^ 1,
            static_cast<int>(freezePosition[1]) ^ 1,
            static_cast<int>(freezePosition[2]) ^ 1
        )
    );

    bulletRigidbody->setAngularFactor(
        btVector3(
            static_cast<int>(freezeRotation[0]) ^ 1,
            static_cast<int>(freezeRotation[1]) ^ 1,
            static_cast<int>(freezeRotation[2]) ^ 1
        )
    );
}

btRigidBody* Rigidbody::GetBulletRigidbody()
{
    return bulletRigidbody;
}
