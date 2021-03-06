#include "Physics/PhysicsManager.h"
#include "Physics/BulletDebugDrawer.h"
#include "Scene/Component.h"
#include <glm/gtx/matrix_decompose.hpp>
#include <algorithm>

btDefaultCollisionConfiguration* PhysicsManager::collisionConfiguration;
btCollisionDispatcher* PhysicsManager::dispatcher;
btBroadphaseInterface* PhysicsManager::overlappingPairCache;
btSequentialImpulseConstraintSolver* PhysicsManager::solver;
btDiscreteDynamicsWorld* PhysicsManager::dynamicsWorld;
btAlignedObjectArray<btCollisionShape*> PhysicsManager::collisionShapes;
std::map<btRigidBody*, entt::entity> PhysicsManager::entityMap;

btConvex2dConvex2dAlgorithm::CreateFunc* PhysicsManager::m_convexAlgo2d;
btBox2dBox2dCollisionAlgorithm::CreateFunc* PhysicsManager::m_box2dbox2dAlgo;
btVoronoiSimplexSolver* PhysicsManager::m_simplexSolver;
btMinkowskiPenetrationDepthSolver* PhysicsManager::m_pdSolver;

glm::vec3 PhysicsManager::gravity = glm::vec3(0.0f, -9.81f, 0.0f);

static BulletDebugDrawer* debugDrawer;

void PhysicsManager::Init()
{
    collisionConfiguration = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionConfiguration);

    m_simplexSolver = new btVoronoiSimplexSolver();
    m_pdSolver = new btMinkowskiPenetrationDepthSolver();

    // Enable 2d and 3d collisions
    m_convexAlgo2d = new btConvex2dConvex2dAlgorithm::CreateFunc(m_simplexSolver, m_pdSolver);
    m_box2dbox2dAlgo = new btBox2dBox2dCollisionAlgorithm::CreateFunc();

    dispatcher->registerCollisionCreateFunc(CONVEX_2D_SHAPE_PROXYTYPE, CONVEX_2D_SHAPE_PROXYTYPE, m_convexAlgo2d);
    dispatcher->registerCollisionCreateFunc(BOX_2D_SHAPE_PROXYTYPE, CONVEX_2D_SHAPE_PROXYTYPE, m_convexAlgo2d);
    dispatcher->registerCollisionCreateFunc(CONVEX_2D_SHAPE_PROXYTYPE, BOX_2D_SHAPE_PROXYTYPE, m_convexAlgo2d);
    dispatcher->registerCollisionCreateFunc(BOX_2D_SHAPE_PROXYTYPE, BOX_2D_SHAPE_PROXYTYPE, m_box2dbox2dAlgo);

    overlappingPairCache = new btDbvtBroadphase();
    solver = new btSequentialImpulseConstraintSolver();
    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
    dynamicsWorld->setGravity(btVector3(gravity.x, gravity.y, gravity.z));

    dynamicsWorld->setDebugDrawer(new BulletDebugDrawer());

    debugDrawer = new BulletDebugDrawer();
}

void PhysicsManager::SubmitEntity(Entity entity)
{
    auto transformComponent = entity.GetComponent<TransformComponent>();

    btCompoundShape* rigidShape = new btCompoundShape();
    btCompoundShape* triggerShape = new btCompoundShape();
    if (entity.HasComponent<Colliders>())
    {
        auto colliders = entity.GetComponent<Colliders>().list;
        for (auto collider : colliders)
        {
            if (!collider->isActive) continue;

            collider->UpdateShape(transformComponent.scale);
            btTransform transform;
            transform.setIdentity();
            transform.setOrigin(GlmToBtVec(collider->center));
            if (collider->isTrigger)
            {
                triggerShape->addChildShape(transform, collider->shape);
            }
            else
            {
                rigidShape->addChildShape(transform, collider->shape);
            }
        }
    }

    if (entity.HasComponent<RigidbodyComponent>())
    {
        auto rbComponent = entity.GetComponent<RigidbodyComponent>();
        SharedPtr<Rigidbody> rigidbody = rbComponent.rigidbody;
        rigidbody->Construct(
            transformComponent.position,
            transformComponent.euler,
            transformComponent.scale,
            rigidShape
        );

        // Match entt handle with rigidbody for referencing overlapping objects with triggers
        // TODO downcast to collision object
        uint64_t handle = static_cast<uint64_t>(entt::entity(entity));
        rigidbody->GetBulletRigidbody()->setUserPointer((void*)handle);
        entityMap.emplace(rigidbody->GetBulletRigidbody(), entity);

        dynamicsWorld->addRigidBody(rigidbody->GetBulletRigidbody());

        // NOTE: this needs to be set after its added to dynamics world
        if (!rigidbody->useGravity)
        {
            rigidbody->GetBulletRigidbody()->setGravity(btVector3(0, 0, 0));
        }
    }

    if (triggerShape->getNumChildShapes() > 0)
    {
        if (!entity.HasComponent<TriggerComponent>())
        {
            entity.AddComponent<TriggerComponent>();
        }
        auto triggerComp = entity.GetComponent<TriggerComponent>();
        triggerComp.trigger->Construct(
            transformComponent.position,
            transformComponent.euler,
            transformComponent.scale,
            triggerShape
        );
        dynamicsWorld->addCollisionObject(triggerComp.trigger->GetBulletGhostObject());
        //dynamicsWorld->addCollisionObject(triggerComp.trigger->trigger, btBroadphaseProxy::SensorTrigger, btBroadphaseProxy::StaticFilter);
        dynamicsWorld->getBroadphase()->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());

        triggerComp.trigger->overlapEnterList.clear();
        triggerComp.trigger->overlapExitList.clear();
        triggerComp.trigger->prevList.clear();
    }
    else
    {
        if (entity.HasComponent<TriggerComponent>())
        {
            entity.RemoveComponent<TriggerComponent>();
        }
    }
}

// TODO submit entity instead of just the btRB
//void PhysicsManager::RemoveEntity(btRigidBody* rigidBody)
void PhysicsManager::RemoveEntity(Entity entity)
{
    // TODO should also remove the triggerbody and rigidbody
    if (entity.HasComponent<RigidbodyComponent>())
    {
        btRigidBody* btRB = entity.GetComponent<RigidbodyComponent>().rigidbody->GetBulletRigidbody();
        if (btRB)
        {
            delete btRB->getMotionState();
            delete btRB->getCollisionShape();
            dynamicsWorld->removeRigidBody(btRB);
            delete btRB;
        }

        auto it = entityMap.find(btRB);
        entityMap.erase(it);
    }

    if (entity.HasComponent<TriggerComponent>())
    {
        btCollisionObject* btObj = entity.GetComponent<TriggerComponent>().trigger->GetBulletGhostObject();
        if (btObj)
        {
            dynamicsWorld->removeCollisionObject(btObj);
            delete btObj;
        }
    }


}

std::vector<entt::entity> PhysicsManager::UpdateTrigger(SharedPtr<Trigger>& trigger, const TransformComponent& transformComp)
{
    btTransform transform;
    transform.setIdentity();
    glm::vec3 pos = transformComp.position;
    glm::vec3 triggerPos = pos + trigger->collider->center;
    transform.setOrigin(btVector3(triggerPos.x, triggerPos.y, triggerPos.z));

    glm::vec3 euler = transformComp.euler;
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
    transComp.euler = glm::degrees(glm::eulerAngles(orientation));
    // NOTE: Scale isn't updated since the rb transform doesn't get initialized with that info
    //transComp.scale = scale;
}

void PhysicsManager::DebugDraw()
{
    dynamicsWorld->debugDrawWorld();
    //LOG_INFO("Size of dynamicsWorld = {0}", dynamicsWorld->getNumCollisionObjects());
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
    collisionShapes.clear();

    delete dynamicsWorld;
    delete solver;
    delete overlappingPairCache;
    delete dispatcher;
    delete collisionConfiguration;

    delete m_box2dbox2dAlgo;
    delete m_convexAlgo2d;
    delete m_pdSolver;
    delete m_simplexSolver;

    dynamicsWorld = 0;
    solver = 0;
    overlappingPairCache = 0;
    dispatcher = 0;
    collisionConfiguration = 0;

    m_box2dbox2dAlgo = 0;
    m_convexAlgo2d = 0;
    m_pdSolver = 0;
    m_simplexSolver = 0;
}


// ==============================================================
// Primarily for picking objects in the scene with mouse click
// ==============================================================
// TODO: Bullet already includes a raycast feature, but this requires
// all entities to be submitted to it. This would cause objects withouth rigidbodies to also
// be submitted to dynamicsWorld for processing.
// Maybe use a seperate world which adds all entities so that we can raycast to their AABB
bool PhysicsManager::Raycast(glm::vec3 rayOrigin, glm::vec3 rayDir, uint64_t& handle)
{
    glm::vec3 rayEnd = rayOrigin + rayDir * 10000.0f;

    btVector3 from(rayOrigin.x, rayOrigin.y, rayOrigin.z);
    btVector3 to(rayEnd.x, rayEnd.y, rayEnd.z);
    //debugDrawer->drawLine(from, to, btVector3(1, 0, 1));

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
        handle = (uint64_t)(closestHit.m_collisionObject->getUserPointer());
        CORE_TRACE("Hit Index = {0}", handle);
        return true;
    }

    CORE_INFO("No closest hit");
    return false;
}


