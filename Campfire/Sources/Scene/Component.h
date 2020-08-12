#ifndef COMPONENT_H
#define COMPONENT_H

#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Core/Base.h"
#include "Renderer/Mesh.h"
#include "Scene/Camera.h"
#include "Particles/ParticleSystem.h"
#include "Physics/Rigidbody.h"

struct IDComponent
{
    uint64_t ID;

    operator uint64_t () { return ID; }
};

struct TagComponent
{
    std::string tag;

    operator std::string& () { return tag; }
    //operator const std::string& () const { return tag; }
};

struct TransformComponent
{
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);

    glm::mat4 transform = glm::mat4(1.0f);
    glm::mat4 runtimeTransform = glm::mat4(1.0f);

    glm::mat4 GetTransform()
    {
        transform = glm::mat4(1.0f);

        transform = glm::translate(transform,position);

        glm::quat quaternion = glm::quat(
            glm::vec3(
                glm::radians(rotation.x),
                glm::radians(rotation.y),
                glm::radians(rotation.z)
            )
        );
        glm::mat4 rotation = glm::toMat4(quaternion);
        transform = transform * rotation;

        transform = glm::scale(transform, scale);

        return transform;
    }

    operator glm::mat4& ()
    {
        transform = glm::mat4(1.0f);

        transform = glm::translate(transform,position);

        glm::quat quaternion = glm::quat(
            glm::vec3(
                glm::radians(rotation.x),
                glm::radians(rotation.y),
                glm::radians(rotation.z)
            )
        );
        glm::mat4 rotation = glm::toMat4(quaternion);
        transform = transform * rotation;

        transform = glm::scale(transform, scale);

        runtimeTransform = transform;
        return transform;
    }

};

struct MeshComponent
{
    // Primitive models for default meshes
    enum class Geometry
    {
        CUBE = 0,
        SPHERE,
        PLANE,
        QUAD,
        CONE,
        CYLINDER
    };

    MeshComponent() = default;
    MeshComponent(const std::string& meshPath)
    {
        mesh = Mesh::Create(meshPath);
    }

    MeshComponent(Geometry geometry)
    {
        switch (geometry)
        {
            case Geometry::CUBE:
                mesh = Mesh::Create("../Assets/Models/primitives/cube.fbx");
                break;
            case Geometry::SPHERE:
                mesh = Mesh::Create("../Assets/Models/primitives/sphere.fbx");
                break;
            case Geometry::PLANE:
                mesh = Mesh::Create("../Assets/Models/primitives/plane.fbx");
                break;
            case Geometry::QUAD:
                mesh = Mesh::Create("../Assets/Models/primitives/quad.fbx");
                break;
            case Geometry::CONE:
                mesh = Mesh::Create("../Assets/Models/primitives/cone.fbx");
                break;
            case Geometry::CYLINDER:
                mesh = Mesh::Create("../Assets/Models/primitives/cylinder.fbx");
                break;
        }
    }

    SharedPtr<Mesh> mesh;

    operator SharedPtr<Mesh>& () { return mesh; }
};

struct LightComponent
{
    enum class LightType
    {
        DIRECTIONAL = 0,
        POINT,
        SPOT,
        AREA
    };

    LightComponent() = default;
    LightComponent(LightType t)
        : type(t) {}

    LightType type = LightType::DIRECTIONAL;
    glm::vec4 color = glm::vec4(1.0f);
    // Attenuation factors
    float constant = 1.0f; // Should stay at 1.0f
    float linear = 0.09f;
    float quadratic = 0.032f;
};


struct RigidbodyComponent
{
    SharedPtr<Rigidbody> rigidbody;

    RigidbodyComponent()
    {
        rigidbody = CreateSharedPtr<Rigidbody>();
    }

    operator SharedPtr<Rigidbody>& () { return rigidbody; }
};

struct ColliderComponent
{
    enum Shape
    {
        // Primitive shapes
        Box = 0,
        Sphere,
        Capsule
//        Cylinder,
//        Cone,
//        Plane,
//
//        // Complex shapes
//        Compound,
//        ConvexHull,
//        TriangleMesh,
//        Heightfield,
//        SoftBody,
//        MultiSphere,
//        ConvexPointCloud
    };

    ColliderComponent(Shape shape)
    {
        switch (shape)
        {
            case Box:
                collider = CreateSharedPtr<BoxCollider>();
                break;
            case Sphere:
                collider = CreateSharedPtr<SphereCollider>();
                break;
            case Capsule:
                collider = CreateSharedPtr<CapsuleCollider>();
                break;
        }
    }

    SharedPtr<Collider> collider;
    operator SharedPtr<Collider>& () { return collider; }
};

struct AudioComponent
{
    AudioComponent() = default;
};

struct ScriptComponent
{
    //SharedPtr<Script> script;

    ScriptComponent() = default;
};

struct CameraComponent
{
    SharedPtr<Camera> camera;
    CameraComponent()
    {
        camera = CreateSharedPtr<Camera>();
    }

    operator SharedPtr<Camera>& () { return camera; }
};

struct ParticleSystemComponent
{
    ParticleSystemComponent()
    {
        ps = CreateSharedPtr<ParticleSystem>();
        ps->Init();
    }

    SharedPtr<ParticleSystem> ps;

    operator SharedPtr<ParticleSystem>& () { return ps; }
};

#endif // COMPONENT_H
