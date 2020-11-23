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

glm::vec3 PhysicsManager::gravity = glm::vec3(0.0f, -9.81f, 0.0f);

static BulletDebugDrawer* debugDrawer;

void PhysicsManager::Init()
{
    collisionConfiguration = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionConfiguration);
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

    bool hasRbCollider = false;
    if (entity.HasComponent<Colliders>())
    {
        auto colliders = entity.GetComponent<Colliders>().colliders;
        for (auto collider : colliders)
        {
            if (collider->isTrigger)
            {
                // Create btGhostObject for triggers
                collider->ConstructTrigger(transformComponent.position, transformComponent.euler, transformComponent.scale);
                dynamicsWorld->addCollisionObject(collider->GetBulletGhostObject());
                dynamicsWorld->getBroadphase()->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());

                collider->overlapEnterList.clear();
                collider->overlapExitList.clear();
                collider->prevList.clear();
            }
            else
            {
                hasRbCollider = true;
                SharedPtr<Rigidbody> rigidbody;
                if (entity.HasComponent<RigidbodyComponent>())
                {
                    auto rbComponent = entity.GetComponent<RigidbodyComponent>();
                    rigidbody = rbComponent.rigidbody;
                }
                else
                {
                    // If there is no rigidbody component then the collider
                    // should act statically and not have physics applied to it
                    rigidbody = CreateSharedPtr<Rigidbody>();
                    rigidbody->type = Rigidbody::BodyType::STATIC;
                }

                rigidbody->collider = collider;
                rigidbody->Construct(
                    transformComponent.position,
                    transformComponent.euler,
                    transformComponent.scale
                );

                // Match entt handle with rigidbody for referencing overlapping
                // objects with triggers
                // TODO downcast to collision object
                uint64_t handle = entity.GetComponent<IDComponent>();
                rigidbody->GetBulletRigidbody()->setUserPointer((void*)handle);
                entityMap.emplace(rigidbody->GetBulletRigidbody(), entity);

                dynamicsWorld->addRigidBody(rigidbody->GetBulletRigidbody());
                collider->rigidbodyObject = rigidbody->GetBulletRigidbody();

                // NOTE: this needs to be set after its added to dynamics world
                if (!rigidbody->useGravity)
                {
                    rigidbody->GetBulletRigidbody()->setGravity(btVector3(0, 0, 0));
                }
            }
        }
    }

    if (entity.HasComponent<RigidbodyComponent>()
        && (!hasRbCollider || !entity.HasComponent<Colliders>())
    ) {
        auto rigidbody = entity.GetComponent<RigidbodyComponent>().rigidbody;
        rigidbody->Construct(transformComponent.position, transformComponent.euler, transformComponent.scale);

        dynamicsWorld->addRigidBody(rigidbody->GetBulletRigidbody());

        if (!rigidbody->useGravity)
        {
            rigidbody->GetBulletRigidbody()->setGravity(btVector3(0, 0, 0));
        }

        rigidbody->GetBulletRigidbody()->setCollisionFlags(rigidbody->GetBulletRigidbody()->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
    }

    if (!hasRbCollider)
    {

    }
}

void PhysicsManager::RemoveEntity(Entity entity)
{
    if (entity.HasComponent<RigidbodyComponent>() && !entity.HasComponent<Colliders>())
    {
        SharedPtr<Rigidbody> rigidbody = entity.GetComponent<RigidbodyComponent>().rigidbody;

        // TODO Convert to unique pointer
        btRigidBody* btRB = rigidbody->GetBulletRigidbody();
        if (btRB)
        {
            if (btRB->getMotionState())
                delete btRB->getMotionState();

            delete btRB->getCollisionShape();
            dynamicsWorld->removeRigidBody(btRB);
            delete btRB;
            rigidbody->bulletRigidbody = nullptr;
        }
    }

    if (entity.HasComponent<Colliders>())
    {
        auto& colliders = entity.GetComponent<Colliders>().colliders;
        for (auto& collider : colliders)
        {
            if (!collider->isTrigger)
            {
                btRigidBody* btRB = collider->rigidbodyObject;
                if (btRB)
                {
                    if (btRB->getMotionState())
                        delete btRB->getMotionState();

                    delete btRB->getCollisionShape();
                    dynamicsWorld->removeRigidBody(btRB);
                    delete btRB;
                    collider->rigidbodyObject = nullptr;
                }
            }
            else
            {
                btCollisionObject* obj = collider->GetBulletGhostObject();
                if (obj)
                {
                    dynamicsWorld->removeCollisionObject(obj);
                    delete obj;
                    collider->ghostObject = nullptr;
                }
            }
        }
    }
}

std::vector<entt::entity> PhysicsManager::UpdateTrigger(SharedPtr<Collider>& trigger, const TransformComponent& transformComp)
{
    btTransform transform;
    transform.setIdentity();
    glm::vec3 pos = transformComp.position;
    glm::vec3 triggerPos = pos + trigger->center;
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
    trigger->overlapStayList = overlappingEntities;

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
}

void PhysicsManager::OnUpdate(float dt)
{
    dynamicsWorld->stepSimulation(dt, 10);
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
// NOT USED ANYMORE: but keep in case we want to raycast using bullet for w/e reason
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
