#ifndef BULLET_DEBUG_DRAWER_H
#define BULLET_DEBUG_DRAWER_H

#include "Shader.h"

class BulletDebugDrawer : public btIDebugDraw
{
public:
    struct Line
    {
        Line(btVector3 s, btVector3 e) : start(s), end(e) {}
        btVector3 start, end;
    };

    GLuint VBO, VAO;
    std::vector<Line> debugLines;
    Shader lineShader;

    BulletDebugDrawer()
    {
        // TODO hardcode in for testing, should move to shadercontroller
        lineShader = Shader("../Campfire/Shaders/line.vert", "../Campfire/Shaders/line.frag");
    }

    void SetMatrices(glm::mat4 pViewMatrix, glm::mat4 pProjectionMatrix)
    {
        // TODO refactor and use global buffer
        lineShader.use();
        glUniformMatrix4fv(glGetUniformLocation(lineShader.ID, "proj"), 1, GL_FALSE, glm::value_ptr(pProjectionMatrix));
        glUniformMatrix4fv(glGetUniformLocation(lineShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(pViewMatrix));
        glm::mat4 model = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(lineShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
    }

    void DebugDraw()
    {
        glGenBuffers(1, &VBO);
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(debugLines), &debugLines, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLfloat*)&this->debugLines[0]);
        glEnableVertexAttribArray(0);
        glBindVertexArray(0);

        lineShader.use();
        glBindVertexArray(VAO);
        glDrawArrays(GL_LINES, 0, debugLines.size()*2);
        glBindVertexArray(0);

        debugLines.clear();
    }

    /*
    virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
    {
        debugLines.push_back(Line(from, to));
    }
    */

    virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
    {
        // Vertex data
        GLfloat points[12];

        points[0] = from.x();
        points[1] = from.y();
        points[2] = from.z();
        points[3] = color.x();
        points[4] = color.y();
        points[5] = color.z();

        points[6] = to.x();
        points[7] = to.y();
        points[8] = to.z();
        points[9] = color.x();
        points[10] = color.y();
        points[11] = color.z();

        glDeleteBuffers(1, &VBO);
        glDeleteVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(points), &points, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
        glBindVertexArray(0);

        glBindVertexArray(VAO);
        glDrawArrays(GL_LINES, 0, 2);
        glBindVertexArray(0);
    }

    virtual void drawContactPoint(const btVector3 &, const btVector3 &, btScalar, int, const btVector3 &) {}
    virtual void reportErrorWarning(const char *) {}
    virtual void draw3dText(const btVector3 &, const char *) {}
    virtual void setDebugMode(int p) {
        m = p;
    }
    int getDebugMode(void) const { return 3; }
    int m;
};

#endif // BULLET_DEBUG_DRAWER_H
