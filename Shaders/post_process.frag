#version 460 core

layout(location = 0) in vec2 inUV;

out vec4 fragColor;

//uniform sampler2DMS tex2DMS;
uniform sampler2D sceneTex;
uniform sampler2D bloomBlurTex;
uniform float exposure;

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
    vec3 color =
        texture(sceneTex, inUV).rgb
      + texture(bloomBlurTex, inUV).rgb
    ;

    vec3 result = vec3(1.0f) - exp(-color * exposure);
    result = pow(result, vec3(1.0f / 2.2f));
    fragColor = vec4(result, 1.0f);
}
