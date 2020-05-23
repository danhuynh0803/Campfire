#include "PhysicsManager.h"
#include "Shared.h"
#include <iostream>


void PhysicsManager::Start()
{
    collisionConfiguration = new btDefaultCollisionConfiguration();

    dispatcher = new btCollisionDispatcher(collisionConfiguration);

    overlappingPairCache = new btDbvtBroadphase();

    solver = new btSequentialImpulseConstraintSolver();

    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
    dynamicsWorld->setGravity(btVector3(0, gravity, 0));

    dynamicsWorld->setDebugDrawer(&mydebugdrawer);
}

// TODO for when updating rigidbody parameters in GUI
//void PhysicsManager::UpdateObject()
//{
//
//}

void PhysicsManager::AddObject(GameObject* object)
{
    //NOTE: BtColliders seems to be 2x larger than their openGL scale counterpart
    glm::vec3 scale = object->scale * 0.51f; // 0.51f just to extend collider a bit outside the mesh
    // TODO make collider shape customizable
    btCollisionShape* shape = new btBoxShape(btVector3(scale.x, scale.y, scale.z));

    collisionShapes.push_back(shape);

    btTransform transform;
    transform.setIdentity();
    glm::vec3 pos = object->position;
    transform.setOrigin(btVector3(pos.x, pos.y, pos.z));

    btScalar mass = object->rigidBody->mass;
    btVector3 localInertia(0, 0, 0);
    if (object->rigidBody->isDynamic)
    {
        shape->calculateLocalInertia(mass, localInertia);
    }

    btDefaultMotionState* motionState = new btDefaultMotionState(transform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape, localInertia);
    btRigidBody* body = new btRigidBody(rbInfo);

    dynamicsWorld->addRigidBody(body);
}

//void PhysicsManager::AddObject(GlObject* object)
//{
//    //NOTE: BtColliders seems to be 2x larger than their openGL scale counterpart
//    glm::vec3 scale = object->scale * 0.51f; // 0.51f just to extend collider a bit outside the mesh
//    btCollisionShape* shape = new btBoxShape(btVector3(scale.x, scale.y, scale.z));
//    std::cout << scale.x << scale.y << scale.y << std::endl;
//
//    collisionShapes.push_back(shape);
//
//    btTransform transform;
//    transform.setIdentity();
//    glm::vec3 pos = object->position;
//    transform.setOrigin(btVector3(pos.x, pos.y, pos.z));
//
//    btScalar mass(0.0f);
//    // TODO get rigid body info into scene files
//    if (object->name.compare("Floor") != 0)
//    {
//        mass = 1.0f;
//    }
//
//    bool isDynamic = (mass != 0.f);
//    //object->rigidBody->isDynamic = (mass != 0.f);
//
//    btVector3 localInertia(0, 0, 0);
//    if (isDynamic)
//    {
//        shape->calculateLocalInertia(mass, localInertia);
//    }
//
//    btDefaultMotionState* motionState = new btDefaultMotionState(transform);
//    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape, localInertia);
//    btRigidBody* body = new btRigidBody(rbInfo);
//
//    dynamicsWorld->addRigidBody(body);
//}


GameObject* PhysicsManager::Raycast(glm::vec3 rayOrigin, glm::vec3 rayDir)
{
    glm::vec3 rayEnd = rayOrigin + rayDir * 1000.0f;

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
        std::cout << closestHit.m_collisionObject << '\n';
        return nullptr; // TODO
    }
    else // Not hit with any gameobjects
    {
        std::cout << "No object hit from raycast\n";
        return nullptr;
    }
}

void PhysicsManager::Update()
{
    if (false /*GameStatePaused*/) { return; }

    // Fixed update at 60fps
    dynamicsWorld->stepSimulation(1.0f/60.0f, 10);
    for (int i = dynamicsWorld->getNumCollisionObjects()-1; i >= 0; --i)
    {
        btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
        btRigidBody* body = btRigidBody::upcast(obj);
        btTransform trans;
        if (body && body->getMotionState())
        {
            body->getMotionState()->getWorldTransform(trans);
        }
        else
        {
            trans = obj->getWorldTransform();
        }

        // Update transform
        btScalar m[16];
        trans.getOpenGLMatrix(m);
        //shared.objectManager->objectList[i]->model = glm::make_mat4x4(m);
    }
}

void PhysicsManager::DebugDraw()
{
    mydebugdrawer.SetMatrices(
        shared.renderCamera->GetViewMatrix(),
        // TODO get resolution from camera
        shared.renderCamera->GetProjMatrix(1600.0f, 900.0f)
    );

    dynamicsWorld->debugDrawWorld();
    // TODO make all lines into one draw call
    //mydebugdrawer.DebugDraw();
}

void PhysicsManager::Shutdown()
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
    for (int i = 0; collisionShapes.size(); ++i)
    {
        btCollisionShape* shape = collisionShapes[i];
        collisionShapes[i] = 0;
        delete shape;
    }

    delete dynamicsWorld;
    delete solver;
    delete overlappingPairCache;
    delete dispatcher;
    delete collisionConfiguration;

    collisionShapes.clear();
}
