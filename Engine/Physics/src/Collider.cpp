#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include "Physics/Collider.h"
#include <imgui.h>

//===================================================================
SharedPtr<Collider> Collider::Create(Collider::Shape shape)
{
    SharedPtr<Collider> collider = nullptr;
    switch (shape)
    {
        default: // None case
            collider = CreateSharedPtr<Collider>();
        case Collider::Shape::BOX:
            collider = CreateSharedPtr<BoxCollider>();
            break;
        case Collider::Shape::SPHERE:
            collider = CreateSharedPtr<SphereCollider>();
            break;
        case Collider::Shape::CAPSULE:
            collider = CreateSharedPtr<CapsuleCollider>();
            break;
    }
    return collider;
}


static btVector3 GlmToBtVec(glm::vec3 v)
{
    return btVector3(v.x, v.y, v.z);
}

void Collider::ConstructTrigger(const glm::vec3& pos, const glm::vec3& euler, const glm::vec3& scale)
{
    btTransform transform;
    transform.setIdentity();

    glm::vec3 colliderPos = pos + center;
    transform.setOrigin(GlmToBtVec(colliderPos));

    glm::quat rotation =
        glm::quat(
            glm::vec3(
                glm::radians(euler.x),
                glm::radians(euler.y),
                glm::radians(euler.z)
            )
        );

    btQuaternion quat(rotation.x, rotation.y, rotation.z, rotation.w);
    transform.setRotation(quat);

    UpdateShape(scale);
    ghostObject = new btGhostObject();
    ghostObject->setCollisionShape(shape);
    ghostObject->setWorldTransform(transform);
    ghostObject->setCollisionFlags(ghostObject->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
}


//===================================================================
BoxCollider::BoxCollider()
    //: type(Collider::Shape::BOX)
{
    type = Collider::Shape::BOX;
    shape = new btBoxShape(btVector3(size.x, size.y, size.z));
}

void BoxCollider::UpdateShape(glm::vec3 scale)
{
    shape = new btBoxShape(btVector3(size.x * scale.x, size.y * scale.y, size.z * scale.z));
}

void BoxCollider::ShowData()
{
    ImGui::DragFloat3("Size", (float*)&size, 0.1f);
}

//===================================================================
SphereCollider::SphereCollider()
    //: type(Collider::Shape::SPHERE)
{
    type = Collider::Shape::SPHERE;
    shape = new btSphereShape(radius);
}

void SphereCollider::UpdateShape(glm::vec3 scale)
{
    float maxScale = scale.x > scale.y ? scale.x : scale.y;
    maxScale = scale.z > maxScale ? scale.z : maxScale;
    shape = new btSphereShape(radius * maxScale);
}

void SphereCollider::ShowData()
{
    ImGui::DragFloat("Radius", &radius, 0.1f);
}

//===================================================================
CapsuleCollider::CapsuleCollider()
{
    type = Collider::Shape::CAPSULE;
    //shape = new btCapsuleShape(radius, height, zUp);
}

void CapsuleCollider::UpdateShape(glm::vec3 scale)
{
    //shape = new btSphereShape(radius);
}

void CapsuleCollider::ShowData()
{
    //ImGui::DragFloat("Radius", &radius, 0.1f);
    //ImGui::DragFloat("Height", &height, 0.1f);
}
