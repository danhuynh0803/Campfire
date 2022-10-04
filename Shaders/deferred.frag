#version 450 core
#extension GL_ARB_separate_shader_objects : enable

// =========================================
layout (location = 0) in vec3 inPos;
layout (location = 1) in vec2 inUV;
layout (location = 2) in vec3 inNormal;

// =========================================
layout (location = 0) out vec4 outPosition;
layout (location = 1) out vec4 outAlbedo;
layout (location = 2) out vec4 outNormal;

// =========================================
layout (set = 1, binding = 0) uniform sampler2D uAlbedoMap;
layout (set = 1, binding = 1) uniform sampler2D uMetallicMap;
layout (set = 1, binding = 2) uniform sampler2D uNormalMap;

// =========================================
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

void main()
{
    outPosition = vec4(inPos, 1.0f);
    outAlbedo = texture(uAlbedoMap, inUV);
    outNormal = vec4(GetNormalFromMap(), 1.0f);
}
