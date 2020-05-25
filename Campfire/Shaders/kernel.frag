#version 460 core

in vec2 texCoords;

out vec4 fragColor;

uniform sampler2D screenTex;

const float offset = 1.0f/300.0f;

// outline kernel
float outline[9] = float[]
(
    -1, -1, -1,
    -1,  8, -1,
    -1, -1, -1
);

float gaussian[9] = float[]
(
    1.0/16.0, 1.0/8.0, 1.0/16.0,
    1.0/8.0 , 1.0/4.0, 1.0/8.0 ,
    1.0/16.0, 1.0/8.0, 1.0/16.0
);

float sobelX[9] = float[]
(
    1, 0,-1,
    2, 0,-2,
    1, 0,-1
);

float sobelY[9] = float[]
(
    1, 2, 1,
    0, 0, 0,
   -1,-2,-1
);

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

    // Pass 1
    for (int i = 0; i < 9; ++i)
    {
        col += texture(screenTex, texCoords + offsets[i]).rgb * sobelX[i];
    }

    // Pass 2
    for (int i = 0; i < 9; ++i)
    {
        col += texture(screenTex, texCoords + offsets[i]).rgb * sobelY[i];
    }

    // Gray filter
    col = vec3(0.299*col.r + 0.587*col.g + 0.114*col.b);

    col = texture(screenTex, texCoords).rgb;

    fragColor = vec4(col, 1.0f);
}
