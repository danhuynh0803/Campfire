#version 460 core

in vec2 texCoords;

out vec4 fragColor;

uniform sampler2D screenTex;

const float offset = 1.0f/300.0f;

// outline kernel
float kernel[9] = float[]
(
    -1, -1, -1,
    -1,  8, -1,
    -1, -1, -1
);

// gaussian blur
//float kernel[9] = float[]
//(
//    1.0/16.0, 1.0/8.0, 1.0/16.0,
//    1.0/8.0 , 1.0/4.0, 1.0/8.0 ,
//    1.0/16.0, 1.0/8.0, 1.0/16.0
//);

vec2 offsets[9] = vec2[]
(
    vec2(-offset,  offset), // top-left
    vec2( 0.0f,    offset), // top-center
    vec2( offset,  offset), // top-right
    vec2(-offset,  0.0f),   // center-left
    vec2( 0.0f,    0.0f),   // center-center
    vec2(-offset,  0.0f),   // center-right
    vec2(-offset, -offset), // bottom-left
    vec2( 0.0f,   -offset), // bottom-center
    vec2( offset, -offset)  // bottom-right
);

void main()
{
    vec3 col = vec3(0.0f);
    for (int i = 0; i < 9; ++i)
    {
        col += texture(screenTex, texCoords + offsets[i]).rgb * kernel[i];
    }

    fragColor = vec4(col, 1.0f);
}
