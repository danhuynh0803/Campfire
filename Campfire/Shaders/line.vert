#version 450 core

// =========================================
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

// =========================================
layout (location = 0) out vec3 color;

// =========================================
layout (std140, binding = 0) uniform Camera
{
    mat4 view;
    mat4 proj;
    mat4 viewProj;
};

// =========================================
uniform mat4 model;

// =========================================
void main()
{
    gl_Position = viewProj * model * vec4(aPos, 1.0f);
    color = aColor;
}
