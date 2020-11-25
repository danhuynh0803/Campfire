#include "Physics/Trigger.h"
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

static btVector3 GlmToBtVec(glm::vec3 v)
{
    return btVector3(v.x, v.y, v.z);
}

void Trigger::Construct(const glm::vec3& pos, const glm::vec3& euler, const glm::vec3& scale, btCollisionShape* shape)
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

    ghostObject = new btGhostObject();
    ghostObject->setCollisionShape(shape);
    ghostObject->setWorldTransform(transform);
    ghostObject->setCollisionFlags(ghostObject->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
}


void Trigger::Construct(const glm::vec3& pos, const glm::vec3& euler, const glm::vec3& scale)
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

    collider->UpdateShape(scale);
    ghostObject = new btGhostObject();
    ghostObject->setCollisionShape(collider->shape);
    ghostObject->setWorldTransform(transform);
    ghostObject->setCollisionFlags(ghostObject->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
}


