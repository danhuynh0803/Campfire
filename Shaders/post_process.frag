#version 460 core

layout(location = 0) in vec2 inUV;

out vec4 fragColor;

//uniform sampler2DMS tex2DMS;
uniform sampler2D sceneTex;
uniform sampler2D bloomBlurTex;
uniform float exposure;

void main()
{
    vec3 color =
        texture(sceneTex, inUV).rgb
      + texture(bloomBlurTex, inUV).rgb
    ;

    //color = color / (color + vec3(1.0f));
    //color = pow(color, vec3(1.0f/2.2f));
    fragColor = vec4(vec3(1.0f) - color, 1.0f);

    // apply gamma correction
    //vec3 result = vec3(1.0f) - exp(-color * exposure);
    //result = pow(result, vec3(1.0f / 2.2f));
    //fragColor = vec4(result, 1.0f);
}
