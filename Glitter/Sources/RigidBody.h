#ifndef RIGID_BODY_H
#define RIGID_BODY_H

#include "btBulletDynamicsCommon.h"
#include "Component.h"
#include "GameObject.h"

enum ColShape
{
    COL_BOX,
    COL_SPHERE,
    COL_PLANE,
    COL_CYLINDER,
    COL_CAPSULE,
    COL_CONE
};

class RigidBody : public Component
{
public:

    RigidBody()
    {
//        //NOTE: BtColliders seems to be 2x larger than their openGL scale counterpart
//        glm::vec3 scale = gameObject->scale * 0.51f; // 0.51f just to extend collider a bit outside the mesh
//        // TODO make collider shape customizable
//        btCollisionShape* shape;
//        switch (colShape)
//        {
//            case COL_BOX:
//                shape = new btBoxShape(btVector3(scale.x, scale.y, scale.z));
//                break;
//            case COL_SPHERE:
//                shape = new btSphereShape(scale.x);
//                break;
//            default:
//                shape = new btBoxShape(btVector3(scale.x, scale.y, scale.z));
//                break;
//        }
//
//        btTransform transform;
//        transform.setIdentity();
//        glm::vec3 pos = gameObject->position;
//        transform.setOrigin(btVector3(pos.x, pos.y, pos.z));
//
//        btVector3 localInertia(0, 0, 0);
//        if (isDynamic)
//        {
//            shape->calculateLocalInertia(mass, localInertia);
//        }
//
//        btDefaultMotionState* motionState = new btDefaultMotionState(transform);
//        btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape, localInertia);
//        btRigidBody* body = new btRigidBody(rbInfo);
    }

    float mass = 0.0f;
    ColShape colShape = COL_BOX;
    bool isDynamic = false;

    btCollisionShape* shape = nullptr;
    btRigidBody* body = nullptr;
};

#endif // RIGID_BODY_H
