#include <string>

#include "ObjectManager.h"
#include "ShaderController.h"
#include "Cube.h"
#include "Quad.h"
#include "Light.h"

void ObjectManager::Add(GameObject* object)
{
    objectList.push_back(object);
}

void ObjectManager::RemoveObject(int index)
{
    std::cout << "Removing object at index = " << index << '\n';
    if (!objectList.empty())
    {
        objectList.erase(objectList.begin() + index);
    }
}

void ObjectManager::LoadObject(Geometry geom, std::string name, glm::vec3 pos, glm::vec3 rot, glm::vec3 scale)
{
    GameObject* gameObject = new GameObject();
    GlObject* mesh;

    switch (geom)
    {
        case CUBE: mesh = new Cube(); break;
        case QUAD: mesh = new Quad(); break;
        //case SPHERE: mesh = new Sphere(); break;
        case LIGHT: mesh = new Light(); break;
        case NONE: return;
        default: return;
    }
    mesh->type = geom;
    std::cout << mesh->type << '\n';

    Texture tempTex("Textures/uv.png");
    gameObject->name = name;
    // TODO : refactor somehow?
    if (mesh->type == LIGHT)
        mesh->shader = shaderController.Get("light");
    else
        mesh->shader = shaderController.Get("generic");

    mesh->texture = tempTex;

    gameObject->position = pos;
    gameObject->rotation = rot;
    gameObject->scale = scale;

    gameObject->glObject = mesh;

    // Set up RB
    RigidBody* rb = new RigidBody();
    gameObject->rigidBody = rb;

    objectList.push_back(gameObject);
}

void ObjectManager::Draw(bool isEditor)
{
    int i = 0; // For setting light UBO
    GLuint numLights = 0;
    for (auto objectPtr: objectList)
    {
        if (objectPtr->glObject->type == LIGHT)
        {
            Light* light = static_cast<Light*>(objectPtr->glObject);
            numLights++;

            glm::vec4 color = light->color;
            if (!objectPtr->isActive)
            {
                // If light is inactive, set color to 0
                // this is to overwrite old data stored
                // within the UBO
                color = glm::vec4(0.0f);
            }

            glBindBuffer(GL_UNIFORM_BUFFER, uboLights);

            // =======================================
            // Send in light info to light UBO
            // Position
            glBufferSubData(GL_UNIFORM_BUFFER,
                    3*sizeof(glm::vec4)*i,
                    sizeof(glm::vec4),
                    glm::value_ptr(objectPtr->position));

            // Color
            glBufferSubData(GL_UNIFORM_BUFFER,
                    3*sizeof(glm::vec4)*i + sizeof(glm::vec4),
                    sizeof(glm::vec4),
                    glm::value_ptr(color));

            // Attenuation factors
            glm::vec4 attenuation =
                glm::vec4(
                    light->constant,
                    light->linear,
                    light->quadratic,
                    0.0f // padding
                );

            glBufferSubData(GL_UNIFORM_BUFFER,
                    3*sizeof(glm::vec4)*i + 2*sizeof(glm::vec4),
                    sizeof(glm::vec4),
                    glm::value_ptr(attenuation));
            // =======================================

            glBindBuffer(GL_UNIFORM_BUFFER, 0);
            glBindBufferBase(GL_UNIFORM_BUFFER, 1, uboLights);

            ++i;
        }
        // TODO
        if (objectPtr->isActive)
        {
            if (isEditor)
                objectPtr->glObject->Draw(objectPtr->position, objectPtr->rotation, objectPtr->scale);
            else
                objectPtr->glObject->DrawSim(objectPtr->model);
        }
    }
    // Send number of lights to light UBO
    // TODO replace light UBO with SSBO, since that can store much
    // more data than UBO
    //std::cout << "Number of lights in scene " << numLights << '\n';
    glBindBuffer(GL_UNIFORM_BUFFER, uboLights);
    glBufferSubData(GL_UNIFORM_BUFFER,
            maxNumLights*3*sizeof(glm::vec4),
            sizeof(GLuint),
            &numLights);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, uboLights);
}
