#ifndef COLLIDER_H
#define COLLIDER_H

struct Collider
{
    btCollisionShape* shape = nullptr;
};

struct BoxCollider : public Collider
{
    BoxCollider()
    {
        shape = new btBoxShape(btVector3(size.x, size.y, size.z));
    }
    glm::vec3 size = glm::vec3(1.0f);
};

struct SphereCollider : public Collider
{
    SphereCollider()
    {
        shape = new btSphereShape(radius);
    }

    float radius = 0.5f;
};

struct CapsuleCollider : public Collider
{
    CapsuleCollider()
    {
        //shape = new btCapsuleShape(radius, height, zUp);
    }

    float radius = 0.5f;
    float height = 1.0f;
};

#endif // COLLIDER_H
