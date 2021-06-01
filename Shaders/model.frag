#version 450 core
#extension GL_ARB_separate_shader_objects : enable

const float PI = 3.141592653589793;

// =========================================
layout (location = 0) in vec3 inPos;
layout (location = 1) in vec2 inUV;
layout (location = 2) in vec3 inNormal;

// =========================================
layout (location = 0) out vec4 outColor;

// =========================================
layout (set = 0, binding = 0) uniform Camera
{
    mat4 view;
    mat4 proj;
    mat4 viewProj;
    vec4 pos;
} camera;

// =========================================
// TODO replace with SSBO
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
layout (set = 1, binding = 0) uniform sampler2D uAlbedoMap;
layout (set = 1, binding = 1) uniform sampler2D uMetallicMap;
layout (set = 1, binding = 2) uniform sampler2D uNormalMap;
layout (set = 1, binding = 3) uniform sampler2D uRoughnessMap;
layout (set = 1, binding = 4) uniform sampler2D uAmbientOcclusionMap;
layout (set = 1, binding = 5) uniform sampler2D uEmissiveMap;

layout (set = 1, binding = 6) uniform TextureMapUsage
{
    bool useAlbedoMap;
    bool useMetallicMap;
    bool useNormalMap;
    bool useRoughnessMap;
    bool useOcclusionMap;
    bool useEmissiveMap;
} mapUsage;

// =========================================
float NormalDistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0f);
    float NdotH2 = NdotH * NdotH;

    float denom = PI * pow((NdotH2 * (a2 - 1.0f) + 1.0f), 2);

    return a2 / denom;
}

// =========================================
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0f);
    float k = (r*r) / 8.0f;

    float denom = NdotV * (1.0f - k) + k;

    return NdotV / denom;
}

// =========================================
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0f);
    float NdotL = max(dot(N, L), 0.0f);
    float ggx1 = GeometrySchlickGGX(NdotV, roughness);
    float ggx2 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

// =========================================
vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0f - F0) * pow((1.0f - cosTheta), 5.0f);
}

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

vec3 PhongLighting(vec3 normal)
{
    vec3 color = vec3(0.0f);
    vec2 uv = vec2(inUV.x, 1.0f-inUV.y);
    vec3 albedo = texture(uAlbedoMap, uv).rgb;

    vec3 ambient = 0.5f * albedo;

    // Point lighting
    for (int i = 0; i < lightBuffer.numLights; ++i)
    {
        Light light = lightBuffer.lights[i];
        vec3 lightDir = normalize(light.pos.xyz - inPos);
        float distance = length(light.pos.xyz - inPos);
        float attenuation = 1.0f / (constant + linear*distance + quadratic*(distance*distance));
        vec3 diff = max(0., dot(normal, lightDir)) * light.color.rgb;
        color += light.intensity * attenuation * (ambient + diff*albedo);
    }

    return color;
}

vec3 BasicPbr()
{
    vec3 albedo = mapUsage.useAlbedoMap ? pow(texture(uAlbedoMap, inUV).rgb * vec3(1.0f), vec3(2.2)) : vec3(1.0f);
    // TODO floor of sponza being set to vec3(1.0f) for some reason?
    albedo = pow(texture(uAlbedoMap, inUV).rgb * vec3(1.0f), vec3(2.2));

    if (texture(uAlbedoMap, inUV).a <= 0.01f)
    {
        discard;
    }

    float metallic = mapUsage.useMetallicMap ? texture(uMetallicMap, inUV).r : 0.0f;
    float roughness = mapUsage.useRoughnessMap ? texture(uRoughnessMap, inUV).r : 1.0f;
    float ao = mapUsage.useOcclusionMap ? texture(uAmbientOcclusionMap, inUV).r : 0.1f;

    vec3 emission = mapUsage.useEmissiveMap ? texture(uEmissiveMap, inUV).rgb : vec3(0.0f);
    emission *= 1.0f; //TODO replace with emissiveIntensity
    //emission *= uEmissiveIntensity;

    vec3 N = mapUsage.useNormalMap ? GetNormalFromMap() : normalize(inNormal);

    vec3 V = normalize(camera.pos.xyz - inPos);

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0f);

    for (int i = 0; i < lightBuffer.numLights; ++i)
    {
        Light light = lightBuffer.lights[i];
        vec3 L = normalize(light.pos.xyz - inPos);
        // TODO enable change light type
        //uint type = uint(round(lights[i].attenFactors.w));
        //if (type == 0)
        //{
        //    L = -1.0f * normalize(lights[i].lightDir.xyz);
        //}
        vec3 H = normalize(L + V);

        float attenuation = 1.0f;
        //if (type == 1)
        {
            //vec4 attenFactor = light.attenFactors; // TODO replace atten factors with radius, intensity, etc
            float distance = length(light.pos.xyz - inPos);
            //attenuation = 1.0f / (attenFactor[0] + attenFactor[1]*distance + attenFactor[2]*(distance*distance));
            attenuation = 1.0f / (constant + linear*distance + quadratic*(distance*distance));
            //attenuation = 1.0f / (distance * distance);
        }

        vec3 radiance = light.color.rgb * light.intensity * attenuation;

        // Cook-Torrance BRDF
        float D = NormalDistributionGGX(N, H, roughness);
        float G = GeometrySmith(N, V, L, roughness);
        vec3 F = FresnelSchlick(max(dot(H, V), 0.0f), F0);

        vec3 kS = F;
        vec3 kD = vec3(1.0f) - kS;
        kD *= 1.0f - metallic;

        vec3 numerator = D*F*G;
        float denom = 4 * max(dot(N, L), 0.0f) * max(dot(N, V), 0.0f);

        // skybox color
        //vec3 R = reflect(-V, N);
        //vec3 skyboxColor = texture(skybox, R).rgb;

        //vec3 specular = ( numerator / max(denom, 0.001f) ) * skyboxColor;
        vec3 specular = ( numerator / max(denom, 0.001f) );

        float NdotL = max(dot(N, L), 0.0f);
        //Lo += (kD * albedo/PI + specular) * radiance * NdotL;
        //
        //Lo += max(dot(N, L), 0.0f) * light.color.rgb * attenuation;
        // TODO weird cutoff for light when adding specular and NdotL
        Lo += (kD * albedo/PI) * radiance;
    }

    vec3 ambient = vec3(0.03f) * albedo * ao;
    vec3 color = ambient + Lo + emission;

    // TODO for bloom postprocess
    //float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));
    //if (brightness > 1.0f)
    //    brightColor = vec4(color, 1.0f);
    //else
    //    brightColor = vec4(vec3(0.0f), 1.0f);

    color = color / (color + vec3(1.0f));
    // apply gamma correction
    return pow(color, vec3(1.0f/2.2f));
}

void main()
{
    //outColor = vec4(PhongLighting(normal), 1.0f);
    outColor = vec4(BasicPbr(), 1.0f);
    //outColor = texColor;
}
