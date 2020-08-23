#ifndef COLLIDER_H
#define COLLIDER_H

#include <imgui.h>
#include <glm/glm.hpp>

struct Collider
{
    btCollisionShape* shape = nullptr;
    bool isTrigger = false;
    glm::vec3 center = glm::vec3(0.0f); // Is an offset based from the parent's position

    virtual void UpdateShape() = 0;
    virtual void ShowData() = 0;
};

struct BoxCollider : public Collider
{
    BoxCollider()
    {
        shape = new btBoxShape(btVector3(size.x, size.y, size.z));
    }

    virtual void UpdateShape()
    {
        shape = new btBoxShape(btVector3(size.x, size.y, size.z));
    }

    virtual void ShowData()
    {
        ImGui::DragFloat3("Size", (float*)&size, 0.1f);
    }

    //glm::vec3 size = glm::vec3(1.0f);
    glm::vec3 size = glm::vec3(10, 0.2, 10);
};

struct SphereCollider : public Collider
{
    SphereCollider()
    {
        shape = new btSphereShape(radius);
    }

    virtual void UpdateShape()
    {
        shape = new btSphereShape(radius);
    }

    virtual void ShowData()
    {
        ImGui::DragFloat("Radius", &radius, 0.1f);
    }
    float radius = 1.0f;
};

struct CapsuleCollider : public Collider
{
    CapsuleCollider()
    {
        //shape = new btCapsuleShape(radius, height, zUp);
    }

    virtual void UpdateShape()
    {
        //shape = new btSphereShape(radius);
    }

    virtual void ShowData()
    {
        ImGui::DragFloat("Radius", &radius, 0.1f);
        ImGui::DragFloat("Height", &height, 0.1f);
    }
    float radius = 1.0f;
    float height = 1.0f;
};

#endif // COLLIDER_H
