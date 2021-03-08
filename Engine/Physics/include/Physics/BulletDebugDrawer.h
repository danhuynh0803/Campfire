#ifndef BULLET_DEBUG_DRAWER_H
#define BULLET_DEBUG_DRAWER_H

#include <vector>
#include "Core/Base.h"
#include "Renderer/Shader.h"
#include <btBulletCollisionCommon.h>

class BulletDebugDrawer : public btIDebugDraw
{
public:
    BulletDebugDrawer();
    virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override;
    virtual void drawContactPoint(const btVector3 &, const btVector3 &, btScalar, int, const btVector3 &) {}
    virtual void reportErrorWarning(const char *) {}
    virtual void draw3dText(const btVector3 &, const char *) {}
    virtual void setDebugMode(int p) {
        m = p;
    }

    int getDebugMode(void) const { return 3; }

    struct Line
    {
        Line(btVector3 s, btVector3 e) : start(s), end(e) {}
        btVector3 start, end;
    };

    uint32_t VBO, VAO;
    std::vector<Line> debugLines;
    SharedPtr<Shader> lineShader;
    int m; // TODO what was this for?
};

#endif // BULLET_DEBUG_DRAWER_H
