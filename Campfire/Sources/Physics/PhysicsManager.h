#ifndef PHYSICS_MANAGER_H
#define PHYSICS_MANAGER_H

#include <btBulletDynamicsCommon.h>
#include "Physics/BulletDebugDrawer.h"
//#include "GameObject.h"

class PhysicsManager
{
public:
    static void Start();
    static void Update();
    static void Shutdown();

    static void AddObject(GameObject*);
    static void UpdateColliders();
    static void ClearLists();

    static void DebugDraw();

    static bool Raycast(glm::vec3 rayOrigin, glm::vec3 rayDir, int& index);

public:
    static bool isBoundingBoxOn = false;
    static float gravity = -9.81;

private:
    static btDefaultCollisionConfiguration* collisionConfiguration;
    static btCollisionDispatcher* dispatcher;
    static btBroadphaseInterface* overlappingPairCache;
    static btSequentialImpulseConstraintSolver* solver;
    static btDiscreteDynamicsWorld* dynamicsWorld;
    static btAlignedObjectArray<btCollisionShape*> collisionShapes;
    static BulletDebugDrawer mydebugdrawer;
};

#endif // PHYSICS_MANAGER_H
