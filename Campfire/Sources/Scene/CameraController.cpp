#include "Scene/CameraController.h"
#include "Core/Input.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <imgui.h>

void CameraController::SetActiveCamera(const SharedPtr<Camera>& camera, const glm::vec3& pos, const glm::vec3& euler)
{
    activeCamera = camera;

//    glm::mat4 transform = glm::mat4(1.0f);
//    //transform = glm::translate(transform, pos);
//    glm::quat quaternion = glm::quat(
//        glm::vec3(
//            glm::radians(euler.x),
//            glm::radians(euler.y),
//            glm::radians(euler.z)
//        )
//    );
//    glm::mat4 rotation = glm::toMat4(quaternion);
//    //transform = transform * rotation;
//
//    // Reorient controller parameters based on incoming camera's transform, which is pretty much the view matrix
//    /*
//       Camera view matrix is a column-major, right-hand coordinate system:
//       Rx   Ux   Fx   Tx
//       Ry   Uy   Fy   Ty
//       Rz   Uz   Fz   Tz
//        0    0    0    1
//    */
//    auto view = camera->GetViewMatrix();
//    right    = glm::vec3(view[0][0], view[1][0], view[2][0]);
//    up       = glm::vec3(view[0][1], view[1][1], view[2][1]);
//    front    = glm::vec3(view[0][2], view[1][2], view[2][2]); // Camera faces -Z direction
//    position = glm::vec3(view[0][3], view[1][3], view[2][3]);
//
//    //right    = glm::vec3(transform[0][0], transform[1][0], transform[2][0]);
//    //up       = glm::vec3(transform[0][1], transform[1][1], transform[2][1]);
//    //front    = glm::vec3(transform[0][2], transform[1][2], transform[2][2]) * -1.0f; // Camera faces -Z direction
//
//    //right    = glm::vec3(transform[0][0], transform[0][1], transform[0][2]);
//    //up       = glm::vec3(transform[1][0], transform[1][1], transform[1][2]);
//    //front    = glm::vec3(transform[2][0], transform[2][1], transform[1][2]) * -1.0f; // Camera faces -Z direction
//
//
//    //position = glm::vec3(transform[0][3], transform[1][3], transform[2][3]);
//    position = pos;
//
//    activeCamera->RecalculateViewMatrix(position, front, up);
}

void CameraController::OnEvent(Event& e)
{
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<MouseScrolledEvent>(BIND_EVENT_FN(CameraController::OnMouseScrolled));
    dispatcher.Dispatch<MouseMovedEvent>(BIND_EVENT_FN(CameraController::OnMouseMoved));
}

void CameraController::OnUpdate(float dt)
{
    movementSpeed = (Input::GetKey(KEY_LEFT_SHIFT)) ? maxSpeed : normalSpeed;

    if (Input::GetKey(KEY_W))
        ProcessKeyboard(FORWARD, dt);
    if (Input::GetKey(KEY_A))
        ProcessKeyboard(LEFT, dt);
    if (Input::GetKey(KEY_S))
        ProcessKeyboard(BACKWARD, dt);
    if (Input::GetKey(KEY_D))
        ProcessKeyboard(RIGHT, dt);

    UpdateCameraVectors();
    activeCamera->RecalculateViewMatrix(position, front, up);
}

bool CameraController::OnMouseScrolled(MouseScrolledEvent& e)
{
    if (fov >= 1.0f && fov <= 45.0f)
        fov -= e.GetYOffset();
    if (fov <= 1.0f)
        fov = 1.0f;
    if (fov >= 45.0f)
        fov = 45.0f;

    activeCamera->fov = fov;
    activeCamera->SetProjection();

    return false;
}

bool CameraController::OnWindowResized(WindowResizeEvent& e)
{
    activeCamera->width = e.GetWidth();
    activeCamera->height = e.GetHeight();
    activeCamera->SetProjection();

    return false;
}


bool CameraController::OnMouseMoved(MouseMovedEvent& e)
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
        activeCamera->RecalculateViewMatrix(position, front, up);
    }
    return false;
}

void CameraController::ProcessKeyboard(CameraMovement direction, float deltaTime)
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

void CameraController::UpdateCameraVectors()
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
