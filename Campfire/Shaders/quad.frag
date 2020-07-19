#version 460

// =========================================
layout (location = 1) in vec4 inColor;
layout (location = 2) in vec2 inUV;
layout (location = 3) in float inTexIndex;

// =========================================
out vec4 fragColor;

// =========================================
//uniform sampler2D uTex;
uniform sampler2D uTextures[32];
uniform float time;

// =========================================
void main()
{
    int index = int(inTexIndex);
    fragColor =
        texture(uTextures[index], inUV)
        //texture(uTex, inUV)
        * inColor
    ;
}
