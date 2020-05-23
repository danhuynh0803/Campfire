#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include "RigidBody.h"
#include "GlObject.h"

class GameObject
{
public:
    // TODO think of how to make this a modular system
    //std::vector<Component>();

    RigidBody* rigidBody;
    GlObject* glObject; // Contains the object's render info

    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
    glm::mat4 model = glm::mat4(1.0f);

    std::string name = "unnamed";
    bool isActive = true;
};

#endif // GAME_OBJECT_H
