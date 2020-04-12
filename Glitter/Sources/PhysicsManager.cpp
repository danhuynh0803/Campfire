#include "PhysicsManager.h"
#include "Shared.h"

void PhysicsManager::Start()
{
    collisionConfiguration = new btDefaultCollisionConfiguration();

    dispatcher = new btCollisionDispatcher(collisionConfiguration);

    overlappingPairCache = new btDbvtBroadphase();

    solver = new btSequentialImpulseConstraintSolver();

    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
    dynamicsWorld->setGravity(btVector3(0, -10, 0));
}

void PhysicsManager::AddObject(Object* object)
{

}

void PhysicsManager::AddObject(GlObject* object)
{
    //btCollisionShape* shape = new btBoxShape(btVector3(btScalar(1.0f), btScalar(1.0f), btScalar(1.0f)));
    glm::vec3 scale = object->scale;
    btCollisionShape* shape = new btBoxShape(btVector3(scale.x, scale.y, scale.z));
    collisionShapes.push_back(shape);

    btTransform transform;
    transform.setIdentity();
    glm::vec3 pos = object->position;
    transform.setOrigin(btVector3(pos.x, pos.y, pos.z));

    btScalar mass(0.0f);
    if (object->name.compare("Floor") != 0)
    {
        mass = 1.0f;
    }

    bool isDynamic = (mass != 0.f);
    //object->rigidBody->isDynamic = (mass != 0.f);

    btVector3 localInertia(0, 0, 0);
    if (isDynamic)
    {
        shape->calculateLocalInertia(mass, localInertia);
    }

    btDefaultMotionState* motionState = new btDefaultMotionState(transform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape, localInertia);
    btRigidBody* body = new btRigidBody(rbInfo);

    dynamicsWorld->addRigidBody(body);
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
        shared.objectManager->glObjectList[i]->position =
            glm::vec3((float)trans.getOrigin().getX(),
                      (float)trans.getOrigin().getY(),
                      (float)trans.getOrigin().getZ());
    }
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
