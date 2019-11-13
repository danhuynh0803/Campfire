#version 450 core

in vec2 texCoords;

out vec4 fragColor;

uniform sampler2D tex;

void main()
{
    fragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}
