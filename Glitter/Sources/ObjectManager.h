#ifndef OBJECT_MANAGER_H
#define OBJECT_MANAGER_H

#include <vector>

#include "GlObject.h"

enum Geometry {
    CUBE,
    QUAD,
    SPHERE,
    LIGHT,
    NONE
};

class ObjectManager
{
public:

    ~ObjectManager()
    {
        // TODO crash
        //for (auto objectPtr : objectList)
        //{
        //    delete(objectPtr);
        //}
    }

    void Add(GlObject* object);
    void LoadObject(Geometry geom, std::string name, float pos[3], float rot[3], float scale[3]);
    void RemoveObject(int index);
    void Draw();

    std::vector<GlObject*> objectList;
};

#endif // OBJECT_MANAGER_H
