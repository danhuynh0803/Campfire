#version 460

// =========================================
layout (location = 0) in vec4 inColor;
layout (location = 1) in vec2 inUV;
layout (location = 2) in float inTexIndex;

// =========================================
out vec4 fragColor;

// =========================================
uniform sampler2D uTextures[32];
uniform float time;

// =========================================
void main()
{
    // Round due to floating point errors
    // somehow when moving camera
    // Not sure why this is happening though
    // since this value is passed from vertex data
    int index = int(round(inTexIndex));

    fragColor =
        texture(uTextures[index], inUV)
        * inColor
    ;
}
