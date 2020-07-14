#version 460 core

layout(location = 0) in vec2 inUV;

uniform sampler2D tex2D;
uniform sampler2DMS tex2DMS;
//uniform int samples;

out vec4 fragColor;

void main()
{
    int samples = 16;
    //fragColor = vec4(1.0f) - texture(tex2DMS, inUV);
    ivec2 texCoord = ivec2(textureSize(tex2DMS) * inUV);

    for (int i = 0; i < samples; ++i)
    {
        fragColor += texelFetch(tex2DMS, texCoord, i);
    }
    fragColor *= 1.0f/samples;
}
