#version 450 core

layout(location = 0) in vec2 inUV;

layout(location = 0) out vec4 fragColor;

uniform sampler2D inputImage;
uniform bool isHorizontal;
uniform float weights[5] = float[]
(
    0.227027,
    0.1945946,
    0.1216216,
    0.054054,
    0.016216
);

void main()
{
    vec2 offset = 1.0f / textureSize(inputImage, 0);

    vec3 color = texture(inputImage, inUV).rgb * weights[0];

    int radius = 5;
    if (isHorizontal)
    {
        for (int i = 1; i < radius; ++i)
        {
            color += texture(inputImage, inUV + vec2(offset.x * i, 0.0f)).rgb * weights[i];
            color += texture(inputImage, inUV - vec2(offset.x * i, 0.0f)).rgb * weights[i];
        }
    }
    else
    {
        for (int i = 1; i < radius; ++i)
        {
            color += texture(inputImage, inUV + vec2(0.0f, offset.y * i)).rgb * weights[i];
            color += texture(inputImage, inUV - vec2(0.0f, offset.y * i)).rgb * weights[i];
        }
    }

    fragColor = vec4(color, 1.0f);
}
