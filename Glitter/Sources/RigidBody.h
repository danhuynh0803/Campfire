#ifndef RIGID_BODY_H
#define RIGID_BODY_H

#include "btBulletDynamicsCommon.h"

enum ColShape
{
    COL_CUBE,
    COL_SPHERE,
    COL_CAPSULE
};

class RigidBody
{
public:
    btScalar mass;
    btCollisionShape* colShape;
    bool isDynamic;
};

#endif // RIGID_BODY_H
