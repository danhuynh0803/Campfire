#include <glm/gtc/matrix_transform.hpp>

#include "Core/Log.h"
#include "Scene/Camera.h"
#include "Renderer/Renderer.h"

Camera::Camera()
    : width(1600.0f), height(900.0f), nearPlane(0.1f), farPlane(100.0f)
{
    SetProjection();
    Init();
}

Camera::Camera(float w, float h, float nearplane, float farplane)
    : width(w), height(h), nearPlane(nearplane), farPlane(farplane)
{
    SetProjection();
    Init();
}

void Camera::Init()
{
    shader = ShaderManager::Create("line", "../Campfire/Shaders/line.vert", "../Campfire/Shaders/line.frag");

    /*
       Frustum vertices:

               4--------------------7
              -|                 -  |
            -  |              -     |
          -    |          -         |
        -      |      -             |
       0------------3               |
       |       5----|---------------6
       |     -      |            -
       |   -        |       -
       | -          |   -
       1------------2

    */

    // Create VAO for frustum
    // Vertex data
    GLfloat vertices[] =
    {
        // near
        -1, 1, -nearPlane,
        -1,-1, -nearPlane,
         1,-1, -nearPlane,
         1, 1, -nearPlane,

         // Far
        -1, 1, -farPlane,
        -1,-1, -farPlane,
         1,-1, -farPlane,
         1, 1, -farPlane,
    };

    uint32_t indices[] =
    {
        // near
        0, 1,   1, 2,   2, 3,   3, 0,
        // far
        4, 5,   5, 6,   6, 7,   7, 4,
        // connection
        0, 4,   1, 5,   2, 6,   3, 7
    };

    vertexArray = VertexArray::Create();
    SharedPtr<VertexBuffer> buffer = VertexBuffer::Create(vertices, sizeof(vertices));
    BufferLayout layout =
    {
        { ShaderDataType::FLOAT3, "aPos" }
    };
    buffer->SetLayout(layout);
    vertexArray->AddVertexBuffer(buffer);

    SharedPtr<IndexBuffer> indexBuffer = IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t));

    vertexArray->SetIndexBuffer(indexBuffer);
}

void Camera::DrawFrustum(glm::mat4 transform)
{
    Renderer::DrawLines(shader, vertexArray, transform);
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
