#ifndef OBJECT_MANAGER_H
#define OBJECT_MANAGER_H

#include <vector>

#include "Object.h"

class ObjectManager
{
public:

    ~ObjectManager()
    {
        for (auto objectPtr : glObjectList)
        {
            delete objectPtr;
        }
    }

    void Add(GlObject* object);
    void Add(Object* object);
    void LoadObject(Geometry geom, std::string name, float pos[3], float rot[3], float scale[3]);
    void RemoveObject(int index);
    void Draw();

    std::vector<Object*> objectList;
    std::vector<GlObject*> glObjectList;
    // TODO better way to do this with UBOs?
    // Maybe inside a resource manager?
    GLuint uboLights;
    GLuint maxNumLights = 25; // Make sure this matches with shader

};

#endif // OBJECT_MANAGER_H
