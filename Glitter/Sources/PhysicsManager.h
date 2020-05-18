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

    GameObject* Raycast(glm::vec3 rayOrigin, glm::vec3 rayDir);

private:
    btDefaultCollisionConfiguration* collisionConfiguration;
    btCollisionDispatcher* dispatcher;
    btBroadphaseInterface* overlappingPairCache;
    btSequentialImpulseConstraintSolver* solver;
    btDiscreteDynamicsWorld* dynamicsWorld;
    btAlignedObjectArray<btCollisionShape*> collisionShapes;

    float gravity = -9.81;
};

#endif // PHYSICS_MANAGER_H
