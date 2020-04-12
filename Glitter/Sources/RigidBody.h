#ifndef RIGID_BODY_H
#define RIGID_BODY_H

#include "btBulletDynamicsCommon.h"

enum ColShape
{
    COL_BOX,
    COL_SPHERE,
    COL_CAPSULE
};

class RigidBody
{
public:
    float mass;
    ColShape colShape;
    bool isDynamic;
};

#endif // RIGID_BODY_H
