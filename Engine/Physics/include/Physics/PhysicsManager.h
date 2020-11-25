#ifndef PHYSICS_MANAGER_H
#define PHYSICS_MANAGER_H

#include <vector>
#include <map>

#include <btBulletDynamicsCommon.h>
#include "Physics/Rigidbody.h"
#include "Physics/Trigger.h"
#include "Scene/Entity.h"

static btVector3 GlmToBtVec(glm::vec3 v)
{
    return btVector3(v.x, v.y, v.z);
}

static glm::vec3 btVecToGlm(btVector3 v)
{
    return glm::vec3(v.getX(), v.getY(), v.getZ());
}

class PhysicsManager
{
public:
    static void Init();
    static void OnUpdate(float dt);
    static void Shutdown();

    static void SubmitEntity(Entity entity);
    static void UpdateEntity(SharedPtr<Rigidbody>&, TransformComponent&);
    static std::vector<entt::entity> UpdateTrigger(SharedPtr<Trigger>& trigger, const TransformComponent&);
    // TODO remove generic collision object
    static void RemoveEntity(btRigidBody* rigidBody);
    static void ClearLists();

    static void DebugDraw();

    // TODO Move to general picking or maybe make my own
    static bool Raycast(glm::vec3 rayOrigin, glm::vec3 rayDir, uint64_t& handle);

public:
    //static bool isBoundingBoxOn = false;
    static glm::vec3 gravity;

private:
    // TODO make uniquePtrs for these
    static btDefaultCollisionConfiguration* collisionConfiguration;
    static btCollisionDispatcher* dispatcher;
    static btBroadphaseInterface* overlappingPairCache;
    static btSequentialImpulseConstraintSolver* solver;
    static btDiscreteDynamicsWorld* dynamicsWorld;
    //static btDiscreteDynamicsWorld* raycastWorld;
    static btAlignedObjectArray<btCollisionShape*> collisionShapes;

    //static std::map<btRigidBody*, Entity*> entityMap;
    static std::map<btRigidBody*, entt::entity> entityMap;
};

#endif // PHYSICS_MANAGER_H
