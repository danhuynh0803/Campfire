#version 460

// =========================================
layout (location = 1) in vec4 inColor;
layout (location = 2) in vec2 inUV;

// =========================================
out vec4 fragColor;

// =========================================
uniform sampler2D texIn;
uniform float time;

// =========================================
void main()
{
    //fragColor = texture(texIn, inUV) * tintColor;
    fragColor = inColor;
}
