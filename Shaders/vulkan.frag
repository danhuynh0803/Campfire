#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) in vec3 inColor;
layout (location = 1) in vec2 inUV;

layout (binding = 2) uniform sampler2D uSampler;

layout (location = 0) out vec4 outColor;

void main()
{
    vec4 texColor = texture(uSampler, vec2(inUV.x, -inUV.y));
    //outColor = vec4(inUV, 0.0f, 1.0f);
    outColor = texColor;
}
