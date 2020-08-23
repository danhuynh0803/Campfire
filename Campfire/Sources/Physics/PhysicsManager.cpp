#include "Physics/PhysicsManager.h"
#include "Physics/BulletDebugDrawer.h"
#include <glm/gtx/matrix_decompose.hpp>

btDefaultCollisionConfiguration* PhysicsManager::collisionConfiguration;
btCollisionDispatcher* PhysicsManager::dispatcher;
btBroadphaseInterface* PhysicsManager::overlappingPairCache;
btSequentialImpulseConstraintSolver* PhysicsManager::solver;
btDiscreteDynamicsWorld* PhysicsManager::dynamicsWorld;
btAlignedObjectArray<btCollisionShape*> PhysicsManager::collisionShapes;

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

void PhysicsManager::SubmitEntity(Entity& entity)
{
    auto transformComponent = entity.GetComponent<TransformComponent>();

//    if (!entity.HasComponent<RigidbodyComponent>())
//    {
//        // Although entity does not have a rigidbody component, we must create one in order
//        // for bullet's collisions to simulate.
//        entity.AddComponent<RigidbodyComponent>();
//        auto rigidbody = entity.GetComponent<RigidbodyComponent>().rigidbody;
//        rigidbody->Construct(transformComponent.position, transformComponent.rotation, collider);
//    }

    if (entity.HasComponent<ColliderComponent>() && entity.HasComponent<RigidbodyComponent>())
    {
        auto collider = entity.GetComponent<ColliderComponent>().collider;
        collider->UpdateShape();

        auto rigidbody = entity.GetComponent<RigidbodyComponent>().rigidbody;
        rigidbody->Construct(transformComponent.position, transformComponent.eulerAngles, collider);

        dynamicsWorld->addRigidBody(rigidbody->GetBulletRigidbody());
        if (!rigidbody->useGravity)
        {
            rigidbody->GetBulletRigidbody()->setGravity(btVector3(0, 0, 0));
        }
    }
    else if (entity.HasComponent<ColliderComponent>())
    {
        auto collider = entity.GetComponent<ColliderComponent>().collider;
        collider->UpdateShape();

        // Although entity does not have a rigidbody component, we must create one in order
        // for bullet's collisions to simulate.
        entity.AddComponent<RigidbodyComponent>();
        auto rigidbody = entity.GetComponent<RigidbodyComponent>().rigidbody;
        rigidbody->mass = 0.0f;
        rigidbody->Construct(transformComponent.position, transformComponent.eulerAngles, collider);

        // No rigidbody present so we want to have collisions applied but disable all physics interactions.
        // This is done by setting mass of the rb to 0.
        dynamicsWorld->addRigidBody(rigidbody->GetBulletRigidbody());
        rigidbody->GetBulletRigidbody()->setMassProps(0, btVector3(0, 0, 0));
    }
    else if (entity.HasComponent<RigidbodyComponent>())
    {
        auto rigidbody = entity.GetComponent<RigidbodyComponent>().rigidbody;
        rigidbody->Construct(transformComponent.position, transformComponent.eulerAngles, nullptr);

        dynamicsWorld->addRigidBody(rigidbody->GetBulletRigidbody());

        if (!rigidbody->useGravity)
        {
            rigidbody->GetBulletRigidbody()->setGravity(btVector3(0, 0, 0));
        }
        // Disable collisions since no collider is attached
        rigidbody->GetBulletRigidbody()->setCollisionFlags(rigidbody->GetBulletRigidbody()->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
    }

}

void PhysicsManager::RemoveEntity(btRigidBody* rigidBody)
{
    if (rigidBody)
    {
        delete rigidBody->getMotionState();
        delete rigidBody->getCollisionShape();
        dynamicsWorld->removeRigidBody(rigidBody);
        delete rigidBody;
    }
}

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

    transComp.position = translation;
    transComp.rotation = orientation;
    transComp.eulerAngles = glm::degrees(glm::eulerAngles(orientation));
    transComp.scale = scale;
}

void PhysicsManager::DebugDraw()
{
    dynamicsWorld->debugDrawWorld();
}

void PhysicsManager::OnUpdate(float dt)
{
    dynamicsWorld->stepSimulation(dt, 10);

    /*
    for (size_t i = dynamicsWorld->getNumCollisionObjects()-1; i >= 0; --i)
    {
        btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
        btRigidBody* body = btRigidBody::upcast(obj);

        //btRigidBody* body; //objectPtr->rigidBody->body;
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
        // Apply scale separately since bullet doesn't convey that info
        glm::mat4 transform = glm::make_mat4x4(m);
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
//bool PhysicsManager::Raycast(glm::vec3 rayOrigin, glm::vec3 rayDir, int& index)
//{
//    glm::vec3 rayEnd = rayOrigin + rayDir * 10000.0f;
//
//    btVector3 from(rayOrigin.x, rayOrigin.y, rayOrigin.z);
//    btVector3 to(rayEnd.x, rayEnd.y, rayEnd.z);
//
//    btCollisionWorld::ClosestRayResultCallback closestHit(
//        from,
//        to
//    );
//
//    dynamicsWorld->rayTest(
//        from,
//        to,
//        closestHit
//    );
//
//    if (closestHit.hasHit())
//    {
//        // FIXME
//        //index = static_cast<int>(closestHit.m_collisionObject->getUserPointer());
//        return true;
//    }
//
//    return false;
//}


