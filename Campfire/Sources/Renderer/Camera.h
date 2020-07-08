#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Events/ApplicationEvent.h"
#include "Events/MouseEvent.h"

class Camera
{
public:
    void OnEvent(Event& e);
    void OnUpdate(float timestep);
    bool OnMouseScrolled(MouseScrolledEvent& e);
    bool OnWindowResized(WindowResizeEvent& e);
    bool OnMouseButton(MouseButtonEvent& e);
    bool OnMouseMoved(MouseMovedEvent& e);

public:
    Camera(float width, float height, float near, float far);

    void SetPerspectiveProjection(float width, float height, float near, float far);

    const glm::vec3& GetPosition() const { return position; }
    void SetPosition(const glm::vec3& pos)
    {
        position = pos;
        RecalculateViewMatrix();
    }

    //float GetRotation() const { return rotation; }
    //void SetRotation(float newRotation)
    //{
    //    rotation = newRotation;
    //    RecalculateViewMatrix();
    //}

    glm::mat4 GetViewMatrix() const { return viewMatrix; }
    glm::mat4 GetProjMatrix() const { return projMatrix; }
    glm::mat4 GetViewProjMatrix() const { return projMatrix * viewMatrix; }

private:
    enum CameraMovement
    {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT
    };

    void RecalculateViewMatrix();
    void ProcessKeyboard(CameraMovement direction, float deltaTime);
    void UpdateCameraVectors();

private:
    glm::mat4 viewMatrix;
    glm::mat4 projMatrix;

    float rotation = 0.0f;

    bool isPerspective = true;
    float zoom = 45.0f;
    //float aspectRatio;
    //float nearPlane = 0.1f;
    //float farPlane = 100.0f;
    float maxSpeed = 15.0f;
    float normalSpeed = 2.5f;
    float movementSpeed = normalSpeed;
    float mouseSensitivity = 0.1f;

private: // movement-related
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 front;
    glm::vec3 up = glm::vec3(0, 1, 0);
    glm::vec3 right;
    glm::vec3 worldUp = glm::vec3(0, 1, 0);
    float yaw = -90.0f;
    float pitch = 0.0f;
};

#endif // CAMERA_H
