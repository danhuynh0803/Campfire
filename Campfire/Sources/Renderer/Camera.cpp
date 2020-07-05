#include "Core/Input.h"
#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

#include "Core/Time.h"

void Camera::OnUpdate()
{
    float deltaTime = static_cast<float>(Time::deltaTime);

    if (Input::GetKey(KEY_W))
        ProcessKeyboard(FORWARD, deltaTime);
    if (Input::GetKey(KEY_A))
        ProcessKeyboard(LEFT, deltaTime);
    if (Input::GetKey(KEY_S))
        ProcessKeyboard(BACKWARD, deltaTime);
    if (Input::GetKey(KEY_D))
        ProcessKeyboard(RIGHT, deltaTime);

    UpdateCameraVectors();
    RecalculateViewMatrix();
}

void Camera::ProcessKeyboard(CameraMovement direction, float deltaTime)
{
    float velocity = movementSpeed * deltaTime;
    if (direction == FORWARD)
        position += front * velocity;
    if (direction == BACKWARD)
        position -= front * velocity;
    if (direction == LEFT)
        position -= right * velocity;
    if (direction == RIGHT)
        position += right * velocity;

    // locks the camera at ground level
    //Position.y = 0.0f;
}


bool OnMouseScrolled(MouseScrolledEvent& e);
bool OnWindowResized(WindowResizeEvent& e);

Camera::Camera(float width, float height, float nearPlane, float farPlane)
{
    position = glm::vec3(0.0f, 0.0f, 5.0f);
    front = glm::vec3(0.0f, 0.0f, -1.0f);

    SetPerspectiveProjection(width, height, nearPlane, farPlane);
}

void Camera::SetPerspectiveProjection(float width, float height, float nearPlane, float farPlane)
{
    projMatrix =
        glm::perspective(
            glm::radians(zoom), // actually the vertical FOV
            (width/height),
            nearPlane,
            farPlane
        )
    ;
}

void Camera::RecalculateViewMatrix()
{
    //glm::mat4 transform =
    //    glm::translate(glm::mat4(1.0f), position)
    //    * glm::rotate(glm::mat4(1.0f), glm::radians(rotation), glm::vec3(0, 0, 1))
    //;

    //viewMatrix = glm::inverse(transform);
    //viewProjMatrix = projMatrix * viewMatrix;
    viewMatrix = glm::lookAt(position, position + front, up);
}

void Camera::UpdateCameraVectors()
{
    // Calculate the new Front vector
    glm::vec3 frontVec;
    frontVec.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    frontVec.y = sin(glm::radians(pitch));
    frontVec.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(frontVec);
    // Also re-calculate the Right and Up vector
    right = glm::normalize(glm::cross(front, worldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    up = glm::normalize(glm::cross(right, front));
}
