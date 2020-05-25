#version 450 core

layout (location = 0) in vec3 aPos;

layout (std140) uniform Matrices
{
    mat4 view;
    mat4 projection;
};

uniform mat4 model;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0f);
}
