#include "Physics/Rigidbody.h"
#include "Physics/PhysicsManager.h"

#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

void Rigidbody::Construct(const glm::vec3& pos, const glm::vec3& euler, const SharedPtr<Collider>& collider)
{
    btTransform transform;
    transform.setIdentity();
    transform.setOrigin(btVector3(pos.x, pos.y, pos.z));

    glm::quat rotation = glm::quat(
            glm::vec3(
                glm::radians(euler.x),
                glm::radians(euler.y),
                glm::radians(euler.z)
            )
        );

    btQuaternion quat(rotation.x, rotation.y, rotation.z, rotation.w);
    transform.setRotation(quat);

    btVector3 localInertia(angularDrag, angularDrag, angularDrag);

    btCollisionShape* shape = new btBoxShape(btVector3(0, 0, 0));
    if (collider != nullptr)
    {
        shape = collider->shape;
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
