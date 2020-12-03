#pragma once

#include <glm/glm.hpp>
#include <btBulletDynamicsCommon.h>
#include "Core/Base.h"

struct Collider
{
    enum class Shape
    {
        BOX,
        SPHERE,
        CAPSULE,

        BOX_2D,

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
    virtual void UpdateShape(glm::vec3 scale) = 0;
    virtual void ShowData() = 0;
    virtual void Reset() = 0;

    Shape type = Collider::Shape::BOX;
    bool isTrigger = false;
    btCollisionShape* shape = nullptr;
    glm::vec3 center = glm::vec3(0.0f); // Is an offset based from the parent's position

    //std::string GetShapeTypeString()
    //{
    //    std::string ShapeTypes[] = {"Box","Sphere","Capsule"};
    //    std::string shapeTypeName;
    //    switch (type)
    //    {
    //        case Shape::Box:
    //            shapeTypeName = ShapeTypes[0];
    //            break;
    //        case Shape::Sphere:
    //            shapeTypeName = ShapeTypes[1];
    //            break;
    //        case Shape::Capsule:
    //            shapeTypeName = ShapeTypes[2];
    //            break;
    //    }
    //    return shapeTypeName.append(" Collider");
    //}
};

struct BoxCollider : public Collider
{
    BoxCollider();
    virtual void UpdateShape(glm::vec3 scale);
    virtual void ShowData();
    virtual void Reset()
    {
        size = glm::vec3(1.0f);
        center = glm::vec3(0.0f);
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
        center = glm::vec3(0.0f);
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
        center = glm::vec3(0.0f);
    }

    float radius = 1.0f;
    float height = 1.0f;
};

struct BoxCollider2D : public Collider
{
    BoxCollider2D();
    virtual void UpdateShape(glm::vec3 scale);
    virtual void ShowData();
    virtual void Reset()
    {
        size = glm::vec3(1.0f);
    }

    glm::vec3 size = glm::vec3(1.0f);
};
