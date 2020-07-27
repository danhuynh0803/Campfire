#ifndef CAMERA_CONTROLLER_H
#define CAMERA_CONTROLLER_H

#include "Events/ApplicationEvent.h"
#include "Events/MouseEvent.h"
#include "Camera.h"

class CameraController
{
public:
    void OnEvent(Event& e);
    void OnUpdate(float dt);
    bool OnMouseScrolled(MouseScrolledEvent& e);
    bool OnWindowResized(WindowResizeEvent& e);
    bool OnMouseButton(MouseButtonEvent& e);
    bool OnMouseMoved(MouseMovedEvent& e);

    const glm::vec3& GetPosition() const { return position; }
    void SetPosition(const glm::vec3& pos)
    {
        position = pos;
        //camera.RecalculateViewMatrix();
    }

private:
    enum CameraMovement
    {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT
    };

    glm::vec3 position = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 front;
    glm::vec3 up = glm::vec3(0, 1, 0);
    glm::vec3 right;
    glm::vec3 worldUp = glm::vec3(0, 1, 0);
    float yaw = -90.0f;
    float pitch = 0.0f;

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

private:
    void ProcessKeyboard(CameraMovement direction, float deltaTime);
    void UpdateCameraVectors();


private:
    friend class Camera;
    Camera camera;

    glm::vec3 cameraPosition = glm::vec3(0.0f);
    float cameraTranslationSpeed = 10.0f;
    float cameraRotationSpeed = 180.0f;
};

#endif // CAMERA_CONTROLLER_H
