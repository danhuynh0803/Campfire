#ifndef OBJECT_MANAGER_H
#define OBJECT_MANAGER_H

#include <vector>

#include "GlObject.h"

class ObjectManager
{
public:
    void Add(GlObject* object)
    {
        objectList.push_back(object);
    }

    std::vector<GlObject*> objectList;
};

#endif // OBJECT_MANAGER_H
