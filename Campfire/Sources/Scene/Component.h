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
#include "Particles/ParticleSystem.h"

struct IDComponent
{
    uint64_t ID;
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

    glm::mat4 transform;

    operator glm::mat4& ()
    {
        glm::mat4 transform = glm::mat4(1.0f);

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
    SharedPtr<Mesh> mesh;

    operator SharedPtr<Mesh>& () { return mesh; }
};

struct RigidbodyComponent
{

};

struct AudioComponent
{

};

struct ScriptComponent
{

};

struct CameraComponent
{

};

struct ParticleSystemComponent
{
    SharedPtr<ParticleSystem> ps;
};



#endif // COMPONENT_H
