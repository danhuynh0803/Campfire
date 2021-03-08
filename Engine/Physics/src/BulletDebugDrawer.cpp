#include "Physics/BulletDebugDrawer.h"
#include <glad/glad.h>

BulletDebugDrawer::BulletDebugDrawer()
{
    lineShader = ShaderManager::Get("line");
    lineShader->SetUniformBlock("Camera", 0);
}

void BulletDebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
{
    //LOG_INFO("Drawing Lines");

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

    // TODO get all lines and draw once
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

    lineShader->Bind();
    lineShader->SetMat4("model", glm::mat4(1.0f));
    glBindVertexArray(VAO);
    glDrawArrays(GL_LINES, 0, 2);
    glBindVertexArray(0);
    lineShader->Unbind();
}
