#version 460 core

layout(location = 0) in vec2 posIn;
layout(location = 1) in vec2 texCoordsIn;

out vec2 texCoords;

void main()
{
    gl_Position = vec4(posIn, 0.0f, 1.0f);
    texCoords = texCoordsIn;
}
