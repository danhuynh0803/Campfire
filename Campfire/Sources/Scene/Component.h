#ifndef COMPONENT_H
#define COMPONENT_H

#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Renderer/Mesh.h"
#include "Renderer/Material.h"
#include "Particles/ParticleSystem.h"
#include "Physics/Rigidbody.h"
#include "Physics/Trigger.h"
#include "Scene/Camera.h"
#include "Scene/Entity.h"
#include "Audio/AudioSource.h"

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
    glm::quat rotation = glm::quat(0.0f, 0.0f, 0.0f, 0.0f);
    glm::vec3 eulerAngles = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);
    glm::mat4 transform = glm::mat4(1.0f);

    void Override()
    {
        transform = glm::mat4(1.0f);

        transform = glm::translate(transform,position);

        rotation = glm::quat(
            glm::vec3(
                glm::radians(eulerAngles.x),
                glm::radians(eulerAngles.y),
                glm::radians(eulerAngles.z)
            )
        );
        glm::mat4 rotationMat = glm::toMat4(rotation);
        transform = transform * rotationMat;
        transform = glm::scale(transform, scale);
    }

    operator glm::mat4& ()
    {
        Override();
        return transform;
    }

    void Reset()
    {
        position = glm::vec3(0.0f);
        rotation = glm::quat(0.0f, 0.0f, 0.0f, 0.0f);
        eulerAngles = glm::vec3(0.0f);
        scale = glm::vec3(1.0f);
        transform = glm::mat4(1.0f);
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
        material = MaterialInstance::Create(MaterialInstance::Type::PBR);
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
        material = MaterialInstance::Create(MaterialInstance::Type::PBR);
    }

    void Reset()
    {
    }

    SharedPtr<Mesh> mesh;
    SharedPtr<MaterialInstance> material;

    operator SharedPtr<Mesh>& () { return mesh; }
};

struct SpriteComponent
{
    SpriteComponent()
        : color(glm::vec4(1.0f))
    {
        // Initialize to default white color texture
        sprite = Texture2D::Create(1, 1);
        uint32_t whiteTextureData = 0xffffffff;
        sprite->SetData(&whiteTextureData, sizeof(uint32_t));
    }

    SpriteComponent(const std::string& texPath, const glm::vec4& tint = glm::vec4(1.0f))
        : color(tint)
    {
        sprite = Texture2D::Create(texPath);
    }

    SharedPtr<Texture2D> sprite;
    glm::vec4 color;
    bool flip[2] = { false, false };

    void Reset()
    {
        color = glm::vec4(1.0f);
        flip[0] = false;
        flip[1] = false;
    }
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
    // TODO replace with range and more user friendly light options
    float constant = 1.0f; // Should stay at 1.0f
    float linear = 0.09f;
    float quadratic = 0.032f;

    void Reset()
    {
        color = glm::vec4(1.0f);
        constant = 1.0f; // Should stay at 1.0f
        linear = 0.09f;
        quadratic = 0.032f;
    }
};

struct RigidbodyComponent
{
    RigidbodyComponent()
    {
        rigidbody = CreateSharedPtr<Rigidbody>();
    }

    void Reset()
    {
        rigidbody = CreateSharedPtr<Rigidbody>();
    }

    SharedPtr<Rigidbody> rigidbody;
    operator SharedPtr<Rigidbody>& () { return rigidbody; }
};

struct TriggerComponent
{
    TriggerComponent()
    {
        trigger = CreateSharedPtr<Trigger>();
    }

    void Reset()
    {
        trigger = CreateSharedPtr<Trigger>();
    }

    SharedPtr<Trigger> trigger;
    operator SharedPtr<Trigger>& () { return trigger; }
};

struct AudioComponent
{
    AudioComponent()
    {
        audioSource = CreateSharedPtr<AudioSource>();
    }

    void Reset()
    {
        audioSource = CreateSharedPtr<AudioSource>();
    }

    SharedPtr<AudioSource> audioSource;
    operator SharedPtr<AudioSource>& () { return audioSource; }
};

class ScriptableEntity;
struct NativeScriptComponent
{
    NativeScriptComponent() = default;

    ScriptableEntity* instance = nullptr;

    ScriptableEntity*(*InstantiateScript)();
    void (*DestroyScript)(NativeScriptComponent*);

    template <typename T>
    void Bind()
    {
        InstantiateScript = []() { return static_cast<ScriptableEntity*>(new T()); };
        DestroyScript = [](NativeScriptComponent* nsc) { delete nsc->instance; nsc->instance = nullptr; };
    }

    void Reset()
    {
    }
};

struct ScriptComponent
{
    //SharedPtr<Script> script;

    ScriptComponent() = default;

    void Reset()
    {
    }
};

struct CameraComponent
{
    SharedPtr<Camera> camera;
    CameraComponent()
    {
        camera = CreateSharedPtr<Camera>();
    }

    bool isMain = false;
    operator SharedPtr<Camera>& () { return camera; }

    void Reset()
    {
        isMain = false;
        camera = CreateSharedPtr<Camera>();
    }
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

    void Reset()
    {
        ps = CreateSharedPtr<ParticleSystem>();
        ps->Init();
    }
};

#endif // COMPONENT_H
