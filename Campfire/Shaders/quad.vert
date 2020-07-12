#version 460

// =========================================
layout (location = 0) in vec3 inPos;
layout (location = 1) in vec2 inUV;
layout (location = 2) in vec3 inNormal;

// =========================================
layout (location = 0) out vec2 outUV;

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
    gl_Position =
        viewProj
        * model
        * vec4(inPos, 1.0f);
    outUV = inUV;
}
