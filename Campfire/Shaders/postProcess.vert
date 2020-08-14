#version 460 core

layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aUV;

layout(location = 0) out vec2 outUV;

uniform mat4 model;

void main()
{
    gl_Position = vec4(aPos, 0.0f, 1.0f);
    outUV = aUV;
}
