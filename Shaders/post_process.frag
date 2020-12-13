#version 460 core

layout(location = 0) in vec2 inUV;

uniform sampler2D tex2D;
uniform sampler2DMS tex2DMS;
uniform sampler2D bloomBlur;

out vec4 fragColor;

void main()
{

    //int samples = 16;
    //ivec2 texCoord = ivec2(textureSize(tex2DMS) * inUV);
    //for (int i = 0; i < samples; ++i)
    //{
    //    fragColor += texelFetch(tex2DMS, texCoord, i);
    //}
    //fragColor *= 1.0f/samples;
    //vec4 color = texture(tex2D, inUV);
    //fragColor = vec4(1.0f) - fragColor;
    //
}
