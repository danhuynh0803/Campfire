#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec2 inUV;
layout (location = 2) in vec3 inNormal;

layout (location = 0) out vec4 outColor;

layout (binding = 2) uniform sampler2D uAlbedoMap;
layout (binding = 3) uniform sampler2D uNormalMap;

vec3 lightPos = vec3(0, 0, 0.5);
vec3 lightCol = vec3(0, 1, 1);

layout (binding = 2) uniform Lights
{
    vec3 pos;
    vec3 color;
} lights;

vec3 GetNormalFromMap()
{
    vec2 uv = vec2(inUV.x, 1.0f-inUV.y);
    vec3 tangentNormal = texture(uNormalMap, uv).xyz * 2.0f - 1.0f;

    vec3 q1  = dFdx(inPos);
    vec3 q2  = dFdy(inPos);
    vec2 st1 = dFdx(uv);
    vec2 st2 = dFdy(uv);

    vec3 N = normalize(inNormal);
    vec3 T = normalize(q1*st2.t - q2*st1.t);
    vec3 B = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

vec3 PhongLighting(vec3 normal)
{
    vec3 color = vec3(0.0f);
    vec2 uv = vec2(inUV.x, 1.0f-inUV.y);
    vec3 albedo = texture(uAlbedoMap, uv).rgb;
    vec3 ambient = 0.1f * albedo;

    vec3 lightDir = normalize(lightPos - inPos);
    vec3 diff = max(0., dot(normal, lightDir)) * lightCol;

    color = ambient + diff*albedo;

    return color;
}

void main()
{
    vec2 uv = vec2(inUV.x, 1.0f-inUV.y);
    vec4 texColor = texture(uAlbedoMap, uv);
    vec3 normal = GetNormalFromMap();

    outColor = vec4(PhongLighting(normal), 1.0f);
}
