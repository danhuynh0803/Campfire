#ifndef CAMERA_CONTROLLER_H
#define CAMERA_CONTROLLER_H

#include "Events/ApplicationEvent.h"
#include "Events/MouseEvent.h"
#include "Scene/Camera.h"
#include "Core/Base.h"

class CameraController
{
public:
    SharedPtr<Camera> GetActiveCamera() { return activeCamera; }
    void SetActiveCamera(const SharedPtr<Camera>& camera, const glm::vec3& position = glm::vec3(0.0f), const glm::vec3& rotation = glm::vec3(0.0f));
    void OnEvent(Event& e);
    void OnUpdate(float dt);
    bool OnMouseScrolled(MouseScrolledEvent& e);
    bool OnWindowResized(WindowResizeEvent& e);
    //bool OnMouseButton(MouseButtonEvent& e);
    bool OnMouseMoved(MouseMovedEvent& e);

public:
    float maxSpeed = 15.0f;
    float normalSpeed = 2.5f;
    float movementSpeed = normalSpeed;
    float mouseSensitivity = 0.1f;

    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);

private:
    SharedPtr<Camera> activeCamera;

    enum CameraMovement
    {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT
    };

    glm::vec3 front = glm::vec3(0, 0, -1);
    glm::vec3 up = glm::vec3(0, 1, 0);
    glm::vec3 right = glm::vec3(1, 0, 0);
    glm::vec3 worldUp = glm::vec3(0, 1, 0);
    float fov = 60.0f;
    float yaw = -90.0f;
    float pitch = 0.0f;

private:
    void ProcessKeyboard(CameraMovement direction, float deltaTime);
    void UpdateCameraVectors();
};

#endif // CAMERA_CONTROLLER_H
