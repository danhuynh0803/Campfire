#version 460

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec2 inUV;

layout (location = 0) out vec2 outUV;

uniform mat4 model;

void main()
{
    gl_Position = model * vec4(inPos, 1.0f);
    outUV = inUV;
}
