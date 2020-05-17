#ifndef PHYSICS_MANAGER_H
#define PHYSICS_MANAGER_H

#include "btBulletDynamicsCommon.h"
#include "GameObject.h"

class PhysicsManager
{
public:
    void Start();
    void Update();
    void Shutdown();
    void AddObject(GameObject*);
    void AddObject(GlObject*);

private:
    btDefaultCollisionConfiguration* collisionConfiguration;
    btCollisionDispatcher* dispatcher;
    btBroadphaseInterface* overlappingPairCache;
    btSequentialImpulseConstraintSolver* solver;
    btDiscreteDynamicsWorld* dynamicsWorld;

    btAlignedObjectArray<btCollisionShape*> collisionShapes;
};

#endif // PHYSICS_MANAGER_H
