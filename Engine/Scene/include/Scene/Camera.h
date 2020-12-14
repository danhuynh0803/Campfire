#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Events/ApplicationEvent.h"
#include "Events/MouseEvent.h"
#include "Renderer/VertexArray.h"
#include "Renderer/Shader.h"

enum class ClearFlag
{
    SKYBOX = 0,
    COLOR,
    DEPTH,
    NONE,
};

class Camera
{
public:
    Camera();
    Camera(float w, float h, float nearplane, float farplane);
    void Init();

    void DrawFrustum(glm::mat4 transform);

    void SetProjection();
    void SetProjection(uint32_t newWidth, uint32_t newHeight);
    glm::mat4 GetViewMatrix() const { return viewMatrix; }
    glm::mat4 GetProjMatrix() const { return projMatrix; }
    glm::mat4 GetProjUIMatrix() const { return uiProjMatrix; }
    glm::mat4 GetViewProjMatrix() const { return projMatrix * viewMatrix; }
    void RecalculateViewMatrix(const glm::vec3& position, const glm::vec3& front, const glm::vec3& up);
    void RecalculateViewMatrix(const glm::vec3& position, const glm::vec3& euler);

public:
    ClearFlag clearFlag = ClearFlag::SKYBOX;
    glm::vec4 backgroundColor = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);

    // TODO remove later
    // might be used by particle system?
    glm::vec3 pos = glm::vec3(0.0f);

    uint32_t targetDisplay = 0;
    bool isPerspective = true;
    float vFov = 60.0f;
    float nearPlane = 0.1f;
    float farPlane = 100.0f;
    float depth = 0.0f;
    // viewport rect
    float x = 0, y = 0, width = 1920.0f, height = 1080.0f;
    // Vertical size for ortho viewport
    float size = 5;

private:
    void SetPerspectiveProjection();
    void SetOrthographicProjection();

    glm::mat4 viewMatrix = glm::mat4(1.0f);
    glm::mat4 projMatrix = glm::mat4(1.0f);
    glm::mat4 uiProjMatrix = glm::mat4(1.0f);

    SharedPtr<VertexArray> vertexArray;
    SharedPtr<VertexBuffer> vertexBuffer;
    SharedPtr<Shader> shader;
};
