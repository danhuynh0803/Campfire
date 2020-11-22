#include "Physics/Rigidbody.h"
#include "Physics/PhysicsManager.h"
#include "Scene/Component.h"
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

static btVector3 GlmToBtVec(glm::vec3 v)
{
    return btVector3(v.x, v.y, v.z);
}

static glm::vec3 btVecToGlm(btVector3 v)
{
    return glm::vec3(v.getX(), v.getY(), v.getZ());
}


void Rigidbody::SetTransform(const TransformComponent& transformComp)
{
    btTransform transform;
    transform.setIdentity();
    glm::vec3 pos = transformComp.position;

    CORE_INFO("RB Transform ({0}, {1}, {2})", pos.x, pos.y, pos.z);
    transform.setOrigin(GlmToBtVec(pos));

    glm::vec3 euler = transformComp.euler;
    glm::quat rotation = glm::quat(
            glm::vec3(
                glm::radians(euler.x),
                glm::radians(euler.y),
                glm::radians(euler.z)
            )
        );

    btQuaternion quat(rotation.x, rotation.y, rotation.z, rotation.w);
    transform.setRotation(quat);

    bulletRigidbody->setWorldTransform(transform);
    bulletRigidbody->getMotionState()->setWorldTransform(transform);
    bulletRigidbody->setCenterOfMassTransform(transform);
}

void Rigidbody::Construct(const glm::vec3& pos, const glm::vec3& euler, const glm::vec3& scale)
{
    btTransform transform;
    transform.setIdentity();
    glm::vec3 colliderPos = pos + collider->center;
    transform.setOrigin(GlmToBtVec(colliderPos));

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
    if (collider->type != Collider::Shape::NONE)
    {
        collider->UpdateShape(scale);
        shape = collider->shape;
    }
    shape->calculateLocalInertia(mass, localInertia);

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

    switch (type)
    {
        case (BodyType::STATIC):
        {
            bulletRigidbody->setMassProps(0, btVector3(0, 0, 0));
            break;
        }
        case (BodyType::KINEMATIC):
        {
            bulletRigidbody->setActivationState(DISABLE_DEACTIVATION);
            break;
        }
        case (BodyType::DYNAMIC):
        {
            break;
        }
    }
}

void Rigidbody::SetVelocity(glm::vec3 newVelocity)
{
    bulletRigidbody->setLinearVelocity(GlmToBtVec(newVelocity));
}

void Rigidbody::AddVelocity(glm::vec3 velocity)
{
    btVector3 currentVelocity = bulletRigidbody->getLinearVelocity();

    glm::vec3 newVelocity = velocity + glm::vec3(currentVelocity.x(), currentVelocity.y(), currentVelocity.z());
    bulletRigidbody->setLinearVelocity(GlmToBtVec(newVelocity));
}

glm::vec3 Rigidbody::GetVelocity()
{
    return btVecToGlm(bulletRigidbody->getLinearVelocity());
}

btRigidBody* Rigidbody::GetBulletRigidbody()
{
    return bulletRigidbody;
}
