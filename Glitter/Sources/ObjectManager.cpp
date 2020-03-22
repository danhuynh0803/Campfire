#include "ObjectManager.h"

#include <string>
#include "ShaderController.h"
#include "Cube.h"
#include "Quad.h"
#include "Light.h"

void ObjectManager::Add(GlObject* object)
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

void ObjectManager::LoadObject(Geometry geom, std::string name, float pos[3], float rot[3], float scale[3])
{

    GlObject* object;
    switch (geom)
    {
        case CUBE: object = new Cube(); break;
        case QUAD: object = new Quad(); break;
        //case SPHERE: static_cast<Cube*>(object); break;
        case LIGHT: object = new Light(); break;
        case NONE: return;
        default: return;
    }

    Texture tempTex("Textures/wall.jpg");
    object->name = name;
    object->shader = shaderController.Get("generic");
    object->texture = tempTex;
    object->position = glm::make_vec3(pos);
    object->rotation = glm::make_vec3(rot);
    object->scale = glm::make_vec3(scale);

    objectList.push_back(object);
}

void ObjectManager::Draw()
{
    for (auto objectPtr: objectList)
    {
        objectPtr->Draw();
    }
}
