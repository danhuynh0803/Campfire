#version 450 core

// =========================================
layout (location = 0) in vec3 color;

// =========================================
out vec4 fragColor;

// =========================================
void main()
{
    fragColor = vec4(color, 1.0f);
}
