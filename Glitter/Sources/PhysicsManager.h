#ifndef PHYSICS_MANAGER_H
#define PHYSICS_MANAGER_H

#include "btBulletDynamicsCommon.h"
#include "GameObject.h"
#include "BulletDebugDrawer_OpenGL.h"

class PhysicsManager
{
public:
    void Start();
    void Update();
    void Shutdown();
    void AddObject(GameObject*);
    void AddObject(GlObject*);
    void DebugDraw();

    GameObject* Raycast(glm::vec3 rayOrigin, glm::vec3 rayDir);

    bool isBoundingBoxOn = false;

private:
    btDefaultCollisionConfiguration* collisionConfiguration;
    btCollisionDispatcher* dispatcher;
    btBroadphaseInterface* overlappingPairCache;
    btSequentialImpulseConstraintSolver* solver;
    btDiscreteDynamicsWorld* dynamicsWorld;
    btAlignedObjectArray<btCollisionShape*> collisionShapes;

    BulletDebugDrawer_OpenGL mydebugdrawer;

    //float gravity = -1;
    float gravity = -9.81;
    //float gravity = 0.0f; // TODO Debug option later
};

#endif // PHYSICS_MANAGER_H
