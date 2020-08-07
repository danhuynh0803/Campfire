#include "Core/Input.h"
#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

#include "Core/Base.h"
#include "Core/Time.h"
#include <functional>

void Camera::OnEvent(Event& e)
{
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<MouseScrolledEvent>(BIND_EVENT_FN(Camera::OnMouseScrolled));
    dispatcher.Dispatch<MouseMovedEvent>(BIND_EVENT_FN(Camera::OnMouseMoved));
}

bool Camera::OnMouseMoved(MouseMovedEvent& e)
{
    static bool constrainPitch = true;
    static bool firstMouse = true;
    static float lastX, lastY;

    if (firstMouse)
    {
        lastX = e.GetX();
        lastY = e.GetY();
        firstMouse = false;
    }

    float xOffset = e.GetX() - lastX;
    float yOffset = e.GetY() - lastY;

    lastX = e.GetX();
    lastY = e.GetY();

    if (Input::GetMouseButtonDown(MOUSE_BUTTON_RIGHT))
    {
        xOffset *= mouseSensitivity;
        yOffset *= mouseSensitivity;

        yaw += xOffset;
        pitch -= yOffset;

        // Make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (pitch > 89.0f)
                pitch = 89.0f;
            if (pitch < -89.0f)
                pitch = -89.0f;
        }

        UpdateCameraVectors();
        RecalculateViewMatrix();
    }
    return false;
}

void Camera::OnUpdate(float timestep)
{
    if (Input::GetKey(KEY_LEFT_SHIFT))
    {
        movementSpeed = maxSpeed;
    }
    else
    {
        movementSpeed = normalSpeed;
    }

    if (Input::GetMouseButtonDown(MOUSE_BUTTON_RIGHT))
    {
        if (Input::GetKey(KEY_W))
            ProcessKeyboard(FORWARD, timestep);
        if (Input::GetKey(KEY_A))
            ProcessKeyboard(LEFT, timestep);
        if (Input::GetKey(KEY_S))
            ProcessKeyboard(BACKWARD, timestep);
        if (Input::GetKey(KEY_D))
            ProcessKeyboard(RIGHT, timestep);
    }

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


bool Camera::OnMouseScrolled(MouseScrolledEvent& e)
{
    if (zoom >= 1.0f && zoom <= 45.0f)
        zoom -= e.GetYOffset();
    if (zoom <= 1.0f)
        zoom = 1.0f;
    if (zoom >= 45.0f)
        zoom = 45.0f;

    SetPerspectiveProjection(viewportWidth, viewportHeight, 0.1f, 100.0f);

    return false;
}

bool Camera::OnWindowResized(WindowResizeEvent& e)
{
    viewportWidth = e.GetWidth();
    viewportHeight = e.GetHeight();

    SetPerspectiveProjection(viewportWidth, viewportHeight, 0.1f, 100.0f);

    return false;
}


Camera::Camera(float width, float height, float nearPlane, float farPlane)
    : viewportWidth(width), viewportHeight(height)
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
