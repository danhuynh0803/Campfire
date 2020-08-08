#include "Scene/Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include "Core/Log.h"

Camera::Camera()
    : width(1600.0f), height(900.0f), nearPlane(0.1f), farPlane(1000.0f)
{
    SetProjection();
}

Camera::Camera(float w, float h, float nearplane, float farplane)
    : width(w), height(h), nearPlane(nearplane), farPlane(farplane)
{
    SetProjection();
}


void Camera::RecalculateViewMatrix(const glm::vec3& position, const glm::vec3& front, const glm::vec3& up)
{
    viewMatrix = glm::lookAt(position, position + front, up);
}

void Camera::SetProjection()
{
    if (isPerspective)
        SetPerspectiveProjection();
    else
        SetOrthographicProjection();
}

void Camera::SetPerspectiveProjection()
{
    projMatrix =
        glm::perspective(
            glm::radians(fov), // actually the vertical FOV
            (width / height), // aspect ratio
            nearPlane,
            farPlane
        )
    ;
}

void Camera::SetOrthographicProjection()
{
    float aspectRatio = (width / height);
    projMatrix =
        glm::ortho(
            -aspectRatio * size, // min X
             aspectRatio * size, // max X
            -size,               // min Y
             size,               // max Y
            nearPlane,           // min Z
            farPlane             // max Z
        )
    ;
}
