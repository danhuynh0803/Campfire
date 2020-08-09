#ifndef COLLIDER_H
#define COLLIDER_H

#include <imgui.h>

struct Collider
{
    btCollisionShape* shape = nullptr;
    bool isTrigger = false;
    glm::vec3 center = glm::vec3(0.0f); // Is an offset based from the parent's position

    virtual void ShowData() = 0;
};

struct BoxCollider : public Collider
{
    BoxCollider()
    {
        shape = new btBoxShape(btVector3(size.x, size.y, size.z));
    }
    glm::vec3 size = glm::vec3(1.0f);

    virtual void ShowData()
    {
        ImGui::DragFloat3("Size", (float*)&size);
    }
};

struct SphereCollider : public Collider
{
    SphereCollider()
    {
        shape = new btSphereShape(radius);
    }

    virtual void ShowData()
    {
        ImGui::DragFloat("Radius", &radius);
    }
    float radius = 0.5f;
};

struct CapsuleCollider : public Collider
{
    CapsuleCollider()
    {
        //shape = new btCapsuleShape(radius, height, zUp);
    }

    virtual void ShowData()
    {
        ImGui::DragFloat("Radius", &radius);
        ImGui::DragFloat("Height", &height);
    }
    float radius = 0.5f;
    float height = 1.0f;
};

#endif // COLLIDER_H
