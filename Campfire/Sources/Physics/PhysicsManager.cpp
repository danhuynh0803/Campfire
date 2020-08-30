#include "Physics/PhysicsManager.h"
#include "Physics/BulletDebugDrawer.h"
#include "Physics/Trigger.h"
#include <glm/gtx/matrix_decompose.hpp>

#include <algorithm>

btDefaultCollisionConfiguration* PhysicsManager::collisionConfiguration;
btCollisionDispatcher* PhysicsManager::dispatcher;
btBroadphaseInterface* PhysicsManager::overlappingPairCache;
btSequentialImpulseConstraintSolver* PhysicsManager::solver;
btDiscreteDynamicsWorld* PhysicsManager::dynamicsWorld;
btAlignedObjectArray<btCollisionShape*> PhysicsManager::collisionShapes;
std::map<btRigidBody*, entt::entity> PhysicsManager::entityMap;

glm::vec3 PhysicsManager::gravity = glm::vec3(0.0f, -9.81f, 0.0f);

void PhysicsManager::Init()
{
    collisionConfiguration = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionConfiguration);
    overlappingPairCache = new btDbvtBroadphase();
    solver = new btSequentialImpulseConstraintSolver();
    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
    dynamicsWorld->setGravity(btVector3(gravity.x, gravity.y, gravity.z));

    dynamicsWorld->setDebugDrawer(new BulletDebugDrawer());
}

void PhysicsManager::SubmitEntity(Entity entity)
{
    auto transformComponent = entity.GetComponent<TransformComponent>();
    if (entity.HasComponent<RigidbodyComponent>())
    {
        auto rbComponent = entity.GetComponent<RigidbodyComponent>();
        SharedPtr<Rigidbody> rigidbody = rbComponent.rigidbody;
        rigidbody->Construct(
            transformComponent.position,
            transformComponent.eulerAngles,
            transformComponent.scale
        );

        // Match entt handle with rigidbody for referencing overlapping objects with triggers
        // TODO downcast to collision object
        entityMap.emplace(rigidbody->GetBulletRigidbody(), entity);

        dynamicsWorld->addRigidBody(rigidbody->GetBulletRigidbody());

        // NOTE: this needs to be set after its added to dynamics world
        if (!rigidbody->useGravity)
        {
            rigidbody->GetBulletRigidbody()->setGravity(btVector3(0, 0, 0));
        }
    }

    if (entity.HasComponent<TriggerComponent>())
    {
        auto triggerComp = entity.GetComponent<TriggerComponent>();
        triggerComp.trigger->Construct(transformComponent.position, transformComponent.eulerAngles, transformComponent.scale);
        dynamicsWorld->addCollisionObject(triggerComp.trigger->GetBulletGhostObject());
        //dynamicsWorld->addCollisionObject(triggerComp.trigger->trigger, btBroadphaseProxy::SensorTrigger, btBroadphaseProxy::StaticFilter);
        dynamicsWorld->getBroadphase()->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());

        triggerComp.trigger->overlapEnterList.clear();
        triggerComp.trigger->overlapExitList.clear();
        triggerComp.trigger->prevList.clear();
    }
}

void PhysicsManager::RemoveEntity(btRigidBody* rigidBody)
{
    // TODO should also remove the triggerbody and rigidbody
    if (rigidBody)
    {
        delete rigidBody->getMotionState();
        delete rigidBody->getCollisionShape();
        dynamicsWorld->removeRigidBody(rigidBody);
        delete rigidBody;
    }
}

std::vector<entt::entity> PhysicsManager::UpdateTrigger(SharedPtr<Trigger>& trigger, const TransformComponent& transformComp)
{
    btTransform transform;
    transform.setIdentity();
    glm::vec3 pos = transformComp.position;
    glm::vec3 triggerPos = pos + trigger->collider->center;
    transform.setOrigin(btVector3(triggerPos.x, triggerPos.y, triggerPos.z));

    glm::vec3 euler = transformComp.eulerAngles;
    glm::quat rotation = glm::quat(
            glm::vec3(
                glm::radians(euler.x),
                glm::radians(euler.y),
                glm::radians(euler.z)
            )
        );

    btQuaternion quat(rotation.x, rotation.y, rotation.z, rotation.w);
    transform.setRotation(quat);

    auto ghostObject = trigger->GetBulletGhostObject();
    ghostObject->setWorldTransform(transform);

    btAlignedObjectArray<btCollisionObject*> overlappingObjects;
    std::vector<entt::entity> overlappingEntities;
    for (int i = 0; i < ghostObject->getNumOverlappingObjects(); ++i)
    {
        btRigidBody* rb = btRigidBody::upcast(ghostObject->getOverlappingObject(i));
        if (rb)
        {
            entt::entity entity = entityMap[rb];
            overlappingEntities.push_back(entity);
        }
    }

    // Lists needs to be sorted before being used in set_difference
    // prev list should always be sorted since it just copies overlappingEntities at the end
    std::sort(overlappingEntities.begin(), overlappingEntities.end());

    // If there exists an entity in overlappingEntities that isn't in prevList,
    // then that entity has entered the trigger for the first time.
    std::vector<entt::entity> enterDiff;
    std::set_difference(
            overlappingEntities.begin(), overlappingEntities.end(),
            trigger->prevList.begin(), trigger->prevList.end(),
            std::inserter(enterDiff, enterDiff.begin())
    );
    trigger->overlapEnterList = enterDiff;

    // Vice versa, an entity that was in prevList but is now no longer in
    // overlappingEntities means that it has just exitted the trigger.
    std::vector<entt::entity> exitDiff;
    std::set_difference(
            trigger->prevList.begin(), trigger->prevList.end(),
            overlappingEntities.begin(), overlappingEntities.end(),
            std::inserter(exitDiff, exitDiff.begin())
    );
    trigger->overlapExitList = exitDiff;

    trigger->prevList = overlappingEntities;

    return overlappingEntities;
}

// TODO this should probably just move to a rigidbody->OnUpdate(dt)?
void PhysicsManager::UpdateEntity(SharedPtr<Rigidbody>& rb, TransformComponent& transComp)
{
    btRigidBody* body = rb->GetBulletRigidbody();
    btTransform trans;
    if (body && body->getMotionState())
    {
        body->getMotionState()->getWorldTransform(trans);
    }
    else
    {
        //trans = objectPtr->rigidBody->shape->getWorldTransform();
        //std::cout << "Warning: Object missing motion state\n";
    }

    // Update transform
    btScalar m[16];
    trans.getOpenGLMatrix(m);

    glm::mat4 glTransform = glm::make_mat4x4(m);
    glm::vec3 scale;
    glm::quat orientation;
    glm::vec3 translation;
    glm::vec3 skew;
    glm::vec4 perspective;

    glm::decompose(glTransform, scale, orientation, translation, skew, perspective);

    // Position needs to be offset by the collider center
    // or the mesh will always move to where the collider is centered to
    transComp.position = translation - rb->collider->center;
    transComp.rotation = orientation;
    transComp.eulerAngles = glm::degrees(glm::eulerAngles(orientation));
    // NOTE: Scale isn't updated since the rb transform doesn't get initialized with that info
    //transComp.scale = scale;
}

void PhysicsManager::DebugDraw()
{
    dynamicsWorld->debugDrawWorld();
}

void PhysicsManager::OnUpdate(float dt)
{
    dynamicsWorld->stepSimulation(dt, 10);

    /*
    int numManifolds = dynamicsWorld->getDispatcher()->getNumManifolds();
    for (int i = 0; i < numManifolds; ++i)
    {
        btPersistentManifold* contactManifold = dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);

        const btCollisionObject* obA = contactManifold->getBody0();
        const btCollisionObject* obB = contactManifold->getBody1();

        int numContacts = contactManifold->getNumContacts();
        for (int j = 0; j < numContacts; ++j)
        {
            btManifoldPoint& pt = contactManifold->getContactPoint(j);
            if (pt.getDistance() < 0.0f)
            {

            }
        }
    }
    */
}

void PhysicsManager::ClearLists()
{
    // remove the rigidbodies from the dynamics world and delete them
    for (int i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; --i)
    {
        btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
        btRigidBody* body = btRigidBody::upcast(obj);
        if (body && body->getMotionState())
        {
            delete body->getMotionState();
        }
        dynamicsWorld->removeCollisionObject(obj);
        delete obj;
    }

    // Delete collision shapes
    for (int i = 0; i < collisionShapes.size(); ++i)
    {
        //btCollisionShape* shape = collisionShapes[i];
        //collisionShapes[i] = 0;
        //delete shape;
    }

    entityMap.clear();
}

void PhysicsManager::Shutdown()
{
    ClearLists();
    delete dynamicsWorld;
    delete solver;
    delete overlappingPairCache;
    delete dispatcher;
    delete collisionConfiguration;

    collisionShapes.clear();
}


// ==============================================================
// Primarily for picking objects in the scene with mouse click
// ==============================================================
// TODO: Bullet already includes a raycast feature, but this requires
// all entities to be submitted to it. This would cause objects withouth rigidbodies to also
// be submitted to dynamicsWorld for processing.
// Maybe use a seperate world which adds all entities so that we can raycast to their AABB
bool PhysicsManager::Raycast(glm::vec3 rayOrigin, glm::vec3 rayDir, int& index)
{
    glm::vec3 rayEnd = rayOrigin + rayDir * 100000.0f;

    btVector3 from(rayOrigin.x, rayOrigin.y, rayOrigin.z);
    btVector3 to(rayEnd.x, rayEnd.y, rayEnd.z);

    btCollisionWorld::ClosestRayResultCallback closestHit(
        from,
        to
    );

    dynamicsWorld->rayTest(
        from,
        to,
        closestHit
    );

    if (closestHit.hasHit())
    {
        // FIXME
        //index = static_cast<int>(closestHit.m_collisionObject->getUserPointer());
        return true;
    }

    return false;
}


