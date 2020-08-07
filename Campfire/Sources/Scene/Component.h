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
#include "Renderer/Camera.h"
#include "Particles/ParticleSystem.h"

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
    RigidbodyComponent() = default;

    float mass = 0.0f;
    float drag = 0.0f;
    float angularDrag = 0.05f;

    bool useGravity = true;
    bool isKinematic = false;

    bool freezePosition[3] { false, false, false };
    bool freezeRotation[3] { false, false, false };
};

struct ColliderComponent
{
    //btCollisionShape* shape = nullptr;

    bool isTrigger = false;
    glm::vec3 center = glm::vec3(0.0f); // Is an offset based from the parent's position
    glm::vec3 size = glm::vec3(1.0f);
};

struct AudioComponent
{
    AudioComponent() = default;
};

struct ScriptComponent
{
    ScriptComponent() = default;
};

struct CameraComponent
{
    SharedPtr<Camera> camera;
    CameraComponent() = default;

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
