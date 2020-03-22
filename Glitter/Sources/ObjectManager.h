#ifndef OBJECT_MANAGER_H
#define OBJECT_MANAGER_H

#include <vector>

#include "GlObject.h"

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

    void Add(GlObject* object)
    {
        objectList.push_back(object);
    }

    // TODO
    void Remove();

    void Draw()
    {
        for (auto objectPtr: objectList)
        {
            if (objectPtr->isActive)
            {
                objectPtr->Draw();
            }
        }
    }

    std::vector<GlObject*> objectList;
};

#endif // OBJECT_MANAGER_H
