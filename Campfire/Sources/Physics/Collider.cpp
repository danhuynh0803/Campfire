#include "Physics/Collider.h"

#include "Core/Base.h"
#include <imgui.h>
#include <algorithm>

//===================================================================
SharedPtr<Collider> Collider::Create(Collider::Shape shape)
{
    SharedPtr<Collider> collider = nullptr;
    switch (shape)
    {
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
    //float maxScale = std::max({ scale.x, scale.y, scale.z });
    //shape = new btSphereShape(radius * maxScale);
    shape = new btSphereShape(radius);
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
    ImGui::DragFloat("Radius", &radius, 0.1f);
    //ImGui::DragFloat("Height", &height, 0.1f);
}
