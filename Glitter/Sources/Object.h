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
};

#endif // OBJECT_H
