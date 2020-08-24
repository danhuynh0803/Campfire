#include "Physics/Collider.h"

#include <imgui.h>
#include <algorithm>

//===================================================================
BoxCollider::BoxCollider()
{
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
{
    shape = new btSphereShape(radius);
}

void SphereCollider::UpdateShape(glm::vec3 scale)
{
    float maxScale = std::max({scale.x, scale.y, scale.z});
    shape = new btSphereShape(radius * maxScale);
}

void SphereCollider::ShowData()
{
    ImGui::DragFloat("Radius", &radius, 0.1f);
}

//===================================================================
CapsuleCollider::CapsuleCollider()
{
    //shape = new btCapsuleShape(radius, height, zUp);
}

void CapsuleCollider::UpdateShape(glm::vec3 scale)
{
    //shape = new btSphereShape(radius);
}

void CapsuleCollider::ShowData()
{
    ImGui::DragFloat("Radius", &radius, 0.1f);
    ImGui::DragFloat("Height", &height, 0.1f);
}
