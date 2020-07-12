#ifndef COMPONENT_H
#define COMPONENT_H

#include <string>

#include <glm/glm.hpp>

#include "Core/Base.h"

struct IDComponent
{

};

struct TagComponent
{
    std::string tag;

    operator std::string& () { return tag; }
    operator const std::string& () const { return tag; }
};

struct TransformComponent
{
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;

    glm::mat4 transform;
};

struct MeshComponent
{

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

};



#endif // COMPONENT_H
