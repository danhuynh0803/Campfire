#version 460

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

layout (location = 1) out vec3 outColor;

void main()
{
    gl_Position = vec4(aPos, 1.0f);
    outColor = aColor;
}
