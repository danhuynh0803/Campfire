#ifndef OBJECT_H
#define OBJECT_H

#include "RigidBody.h"
#include "GlObject.h"

class Object
{
public:
    // TODO think of how to make this a modular system
    //std::vector<Component>();

    RigidBody* rigidBody;
    GlObject* glObject; // Contains the object's render info

    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
};

#endif // OBJECT_H
