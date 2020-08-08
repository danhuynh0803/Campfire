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

void Camera::RecalculateViewMatrix(const glm::vec3& position, const glm::vec3& euler)
{
    float pitch = euler.x;
    float yaw = -90.0f - euler.y;

    // Calculate the orthonormal basis for the game camera based
    // on it's transform
    glm::vec3 front, up, right;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(front);
    right = glm::normalize(glm::cross(front, glm::vec3(0, 1, 0)));
    up = glm::normalize(glm::cross(right, front));

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
