#version 450

// =========================================
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV;
layout (location = 2) in vec3 aNormal;

// =========================================
layout (location = 0) out vec3 outPos;
layout (location = 1) out vec2 outUV;
layout (location = 2) out vec3 outNormal;

// =========================================
layout (set = 0, binding = 0) uniform Camera
{
    mat4 view;
    mat4 proj;
    mat4 viewProj;
} camera;

layout (set = 2, binding = 0) uniform Transform
{
    mat4 model;
} transform;

// =========================================
void main()
{
    gl_Position = camera.viewProj * transform.model * vec4(aPos, 1.0);

    outPos = vec3(transform.model * vec4(aPos, 1.0f));
    outUV = aUV;
    outNormal = mat3(transpose(inverse(transform.model))) * aNormal;
}
