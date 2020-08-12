#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Events/ApplicationEvent.h"
#include "Events/MouseEvent.h"
#include "Renderer/VertexArray.h"
#include "Renderer/Shader.h"

class Camera
{
public:
    Camera();
    Camera(float w, float h, float nearplane, float farplane);
    void Init();

    void DrawFrustum(glm::mat4 transform);

    void SetProjection();
    glm::mat4 GetViewMatrix() const { return viewMatrix; }
    glm::mat4 GetProjMatrix() const { return projMatrix; }
    glm::mat4 GetViewProjMatrix() const { return projMatrix * viewMatrix; }
    void RecalculateViewMatrix(const glm::vec3& position, const glm::vec3& front, const glm::vec3& up);
    void RecalculateViewMatrix(const glm::vec3& position, const glm::vec3& euler);

public:
    glm::vec3 pos = glm::vec3(0.0f); // TODO remove later
    bool isPerspective = true;
    float vFov = 60.0f;
    float nearPlane = 0.1f;
    float farPlane = 100.0f;
    float depth = 0.0f;
    // viewport rect
    float x = 0, y = 0, width = 1600.0f, height = 900.0f;
    // Vertical size for ortho viewport
    float size = 5;

private:
    void SetPerspectiveProjection();
    void SetOrthographicProjection();

    glm::mat4 viewMatrix = glm::mat4(1.0f);
    glm::mat4 projMatrix = glm::mat4(1.0f);

    SharedPtr<VertexArray> vertexArray;
    SharedPtr<VertexBuffer> vertexBuffer;
    SharedPtr<Shader> shader;
};

#endif // CAMERA_H
