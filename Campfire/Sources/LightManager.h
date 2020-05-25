#ifndef LIGHT_MANAGER_H
#define LIGHT_MANAGER_H

#include <vector>

#include "GlObject.h"
#include "Light.h"

class LightManager
{
public:

    ~LightManager()
    {
        // TODO crash
        //for (auto objectPtr : objectList)
        //{
        //    delete(objectPtr);
        //}
    }

    void Add(Light* object)
    {
        objectList.push_back(object);
    }

    // TODO
    void Remove();


    std::vector<Light*> objectList;
};

#endif // OBJECT_MANAGER_H
