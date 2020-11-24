#version 450 core

// =========================================
layout (location = 0) in vec2 inUV;

// =========================================
out vec4 fragColor;

// =========================================
uniform sampler2D text;
uniform vec4 color;

// =========================================
void main()
{
    vec4 sampled = vec4(1.0f, 1.0f, 1.0f, texture(text, inUV).r);

    fragColor = color * sampled;
}
