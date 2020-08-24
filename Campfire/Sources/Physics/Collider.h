#ifndef COLLIDER_H
#define COLLIDER_H

#include <glm/glm.hpp>
#include <btBulletDynamicsCommon.h>

struct Collider
{
    btCollisionShape* shape = nullptr;
    bool isTrigger = false;
    glm::vec3 center = glm::vec3(0.0f); // Is an offset based from the parent's position

    virtual void UpdateShape(glm::vec3 scale) = 0;
    virtual void ShowData() = 0;
    virtual void Reset() = 0;
};

struct BoxCollider : public Collider
{
    BoxCollider();
    virtual void UpdateShape(glm::vec3 scale);
    virtual void ShowData();
    virtual void Reset()
    {
        size = glm::vec3(1.0f);
        center = glm::vec3(1.0f);
    }

    glm::vec3 size = glm::vec3(1.0f);
};

struct SphereCollider : public Collider
{
    SphereCollider();
    virtual void UpdateShape(glm::vec3 scale);
    virtual void ShowData();
    virtual void Reset()
    {
        radius = 1.0f;
        center = glm::vec3(1.0f);
    }

    float radius = 1.0f;
};

struct CapsuleCollider : public Collider
{
    CapsuleCollider();
    virtual void UpdateShape(glm::vec3 scale);
    virtual void ShowData();
    virtual void Reset()
    {
        radius = 1.0f;
        height = 1.0f;
        center = glm::vec3(1.0f);
    }

    float radius = 1.0f;
    float height = 1.0f;
};

#endif // COLLIDER_H
