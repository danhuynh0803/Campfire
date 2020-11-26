#version 450 core

// =========================================
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV;

// =========================================
layout (location = 0) out vec2 outUV;

// =========================================
layout (std140, binding = 0) uniform Camera
{
    mat4 view;
    mat4 proj;
    mat4 viewProj;
    mat4 projUI;

    vec4 pos;
};

uniform bool isUI;

// =========================================
void main()
{
    outUV = aUV;

    if (isUI)
    {
        gl_Position =
            projUI
            * vec4(aPos, 1.0f)
        ;
    }
    else
    {
        gl_Position =
            viewProj
            * vec4(aPos, 1.0f)
        ;
    }
}
