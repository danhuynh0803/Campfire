#version 450 core

layout (location = 0) in vec3 inPos;

// =========================================
layout (std140, binding = 0) uniform Camera
{
    mat4 view;
    mat4 proj;
    mat4 viewProj;
};

out vec3 TexCoords;

void main()
{
    gl_Position = proj * mat4(mat3(view)) * vec4(inPos, 1.0f);
    TexCoords = inPos;
}
