#ifndef OBJECT_MANAGER_H
#define OBJECT_MANAGER_H

#include <vector>

#include "GameObject.h"

class ObjectManager
{
public:

    ~ObjectManager()
    {
        for (auto objectPtr : objectList)
        {
            delete objectPtr;
        }
        objectList.clear();
    }

    void Add(GameObject* object);
    void ObjectManager::LoadObject(Geometry geom, std::string name, glm::vec3 pos, glm::vec3 rot, glm::vec3 scale);
    void RemoveObject(int index);
    void Draw(bool isEditor);

    std::vector<GameObject*> objectList;

    // TODO better way to do this with UBOs?
    // Maybe inside a resource manager?
    GLuint uboLights;
    GLuint maxNumLights = 25; // Make sure this matches with shader

};

#endif // OBJECT_MANAGER_H
