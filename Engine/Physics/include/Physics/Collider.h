#pragma once

#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <btBulletDynamicsCommon.h>
#include <glm/glm.hpp>
#include <entt.hpp>
#include "Core/Base.h"

class PhysicsManager;

struct Collider
{
    enum class Shape
    {
        BOX,
        SPHERE,
        CAPSULE,

        // TODO
        //Cylinder,
        //Cone,
        //Plane,

        //// Complex shapes
        //Compound,
        //ConvexHull,
        //TriangleMesh,
        //Heightfield,
        //SoftBody,
        //MultiSphere,
        //ConvexPointCloud

        NONE
    };

    static SharedPtr<Collider> Create(Collider::Shape shape);
    virtual void UpdateShape(glm::vec3 scale) {};
    virtual void ShowData() {};
    virtual void Reset() {};
    Shape type = Collider::Shape::NONE;
    glm::vec3 center = glm::vec3(0.0f); // Is an offset based from the parent's position

    // ================================================
    // For converting collider to trigger
    bool isTrigger = false;
    btCollisionShape* shape = nullptr;
    void ConstructTrigger(const glm::vec3& pos, const glm::vec3& euler, const glm::vec3& scale);

    btGhostObject* GetBulletGhostObject() { return ghostObject; }

    std::vector<entt::entity> prevList;
    std::vector<entt::entity> overlapEnterList;
    std::vector<entt::entity> overlapExitList;
    std::vector<entt::entity> overlapStayList;

private:
    btRigidBody* rigidbodyObject = nullptr;
    btGhostObject* ghostObject = nullptr;
    friend class PhysicsManager;
};


struct BoxCollider : public Collider
{
    BoxCollider();
    virtual void UpdateShape(glm::vec3 scale);
    virtual void ShowData();
    virtual void Reset() override
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
