#version 460

// =========================================
layout (location = 0) in vec2 inUV;

// =========================================
out vec4 fragColor;

// =========================================
uniform sampler2D texIn;
uniform vec4 tintColor;
uniform float time;

// =========================================
void main()
{
    fragColor = texture(texIn, inUV) * tintColor;
}
