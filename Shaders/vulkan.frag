#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) in vec2 inUV;
layout (location = 1) in vec3 inNormal;

layout (location = 0) out vec4 outColor;

layout (binding = 2) uniform sampler2D uSampler;

void main()
{
    //vec4 texColor = texture(uSampler, vec2(inUV.x, -inUV.y));
    //outColor = texColor;
    outColor = vec4(inNormal, 1.0f);
}
