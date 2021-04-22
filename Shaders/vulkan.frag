#version 450 core
#extension GL_ARB_separate_shader_objects : enable

// =========================================
layout (location = 0) in vec3 inPos;
layout (location = 1) in vec2 inUV;
layout (location = 2) in vec3 inNormal;

// =========================================
layout (location = 0) out vec4 outColor;

// =========================================
const int MAX_NUM_LIGHTS = 100;
struct Light
{
    vec4 pos;
    vec4 color;
    vec3 dir;
    float intensity;
};

layout (set = 0, binding = 1) uniform Lights
{
    Light lights[MAX_NUM_LIGHTS];
    uint numLights;
} lightBuffer;

float constant = 1.0f;
float linear = 0.09f;
float quadratic = 0.032f;

// =========================================
layout (set = 1, binding = 0) uniform sampler2D uComputeResolve;
layout (set = 1, binding = 1) uniform sampler2D uBlank;

// =========================================
void main()
{
    vec2 uv = vec2(inUV.x, 1.0f-inUV.y);
    outColor = texture(uComputeResolve, uv);
}
