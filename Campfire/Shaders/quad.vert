#version 460

// =========================================
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec2 aUV;

// =========================================
layout (location = 1) out vec4 outColor;
layout (location = 2) out vec2 outUV;

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
        * vec4(aPos, 1.0f);

    outColor = aColor;
    outUV = aUV;
}
