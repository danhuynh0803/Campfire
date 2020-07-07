#ifndef CAMERA_CONTROLLER_H
#define CAMERA_CONTROLLER_H

#include "Events/ApplicationEvent.h"
#include "Events/MouseEvent.h"
#include "Camera.h"

class CameraController
{
public:
    void OnUpdate();
    bool OnMouseScrolled(MouseScrolledEvent& e);
    bool OnWindowResized(WindowResizeEvent& e);
    bool OnMouseButton(MouseButtonEvent& e);

private:
    Camera camera;

    glm::vec3 cameraPosition = glm::vec3(0.0f);
    float cameraTranslationSpeed = 10.0f;
    float cameraRotationSpeed = 180.0f;
};

#endif // CAMERA_CONTROLLER_H
