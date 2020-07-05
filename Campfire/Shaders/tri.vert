#version 460

// =========================================
layout (location = 0) in vec3 inPos;
layout (location = 1) in vec2 inUV;
layout (location = 2) in vec3 inNormal;

// =========================================
layout (location = 0) out vec2 outUV;

// =========================================
layout (std140, binding = 0) uniform Matrices
{
    //mat4 view;
    //mat4 projection;
    mat4 viewProjection;
};

// =========================================
uniform mat4 model;

// =========================================
void main()
{
    gl_Position =
        viewProjection
        * model
        * vec4(inPos, 1.0f);
    outUV = inUV;
}
