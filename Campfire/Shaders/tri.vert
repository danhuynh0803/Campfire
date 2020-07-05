#version 460

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec2 inUV;
layout (location = 2) in vec3 inNormal;

layout (location = 0) out vec2 outUV;

uniform mat4 model;
uniform mat4 viewProjMatrix;

void main()
{
    gl_Position =
        viewProjMatrix
        * model
        * vec4(inPos, 1.0f);
    outUV = inUV;
}
