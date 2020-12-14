#version 450 core

const float PI = 3.141592653589793;

// =========================================
const int MAX_NUM_LIGHTS = 25;

// =========================================
struct Light
{
    vec4 pos;
    vec4 color;
    /* packed into a vec4
    x: constant factor for attenuation
    y: linear factor
    z: quadratic factor
    w: light type */
    vec4 attenFactors;
    // Used for directional and spot lights
    vec4 lightDir;
    float intensity;
};

// =========================================
layout (std140, binding = 1) uniform LightBuffer
{
    Light lights[MAX_NUM_LIGHTS];
    uint numLights;
};

// =========================================
uniform vec4  uAlbedo;
uniform float uMetallic;
uniform float uRoughness;
uniform float uAO;

uniform bool useAlbedoMap;
uniform bool useMetallicMap;
uniform bool useNormalMap;
uniform bool useRoughnessMap;
uniform bool useOcclusionMap;

uniform sampler2D albedoMap;
uniform sampler2D metallicMap;
uniform sampler2D normalMap;
uniform sampler2D roughnessMap;
uniform sampler2D ambientOcclusionMap;

uniform samplerCube skybox;

// =========================================
layout (location = 0) in vec3 inPos;
layout (location = 1) in vec2 inUV;
layout (location = 2) in vec3 inNormal;
layout (location = 3) in vec3 inCamPos;

// =========================================
layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 brightColor;

float NormalDistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0f);
    float NdotH2 = NdotH * NdotH;

    float denom = PI * pow((NdotH2 * (a2 - 1.0f) + 1.0f), 2);

    return a2 / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0f);
    float k = (r*r) / 8.0f;

    float denom = NdotV * (1.0f - k) + k;

    return NdotV / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0f);
    float NdotL = max(dot(N, L), 0.0f);
    float ggx1 = GeometrySchlickGGX(NdotV, roughness);
    float ggx2 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0f - F0) * pow((1.0f - cosTheta), 5.0f);
}

vec3 GetNormalFromMap()
{
    vec3 tangentNormal = texture(normalMap, inUV).xyz * 2.0f - 1.0f;

    vec3 q1  = dFdx(inPos);
    vec3 q2  = dFdy(inPos);
    vec2 st1 = dFdx(inUV);
    vec2 st2 = dFdy(inUV);

    vec3 N = normalize(inNormal);
    vec3 T = normalize(q1*st2.t - q2*st1.t);
    vec3 B = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

// =========================================
void main()
{
    vec3 albedo = useAlbedoMap ? pow(texture(albedoMap, inUV).rgb * uAlbedo.rgb, vec3(2.2)) : uAlbedo.rgb;
    float metallic = useMetallicMap ? texture(metallicMap, inUV).r : uMetallic;
    float roughness = useRoughnessMap ? texture(roughnessMap, inUV).r : uRoughness;
    float ao = useOcclusionMap ? texture(ambientOcclusionMap, inUV).r : uAO;
    vec3 N = useNormalMap ? GetNormalFromMap() : normalize(inNormal);

    vec3 V = normalize(inCamPos - inPos);

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0f);

    for (int i = 0; i < numLights; ++i)
    {
        vec3 L = normalize(lights[i].pos.rgb - inPos);
        uint type = uint(round(lights[i].attenFactors.w));
        if (type == 0)
        {
            L = -1.0f * normalize(lights[i].lightDir.xyz);
        }
        vec3 H = normalize(L + V);

        float attenuation = 1.0f;
        if (type == 1)
        {
            vec4 attenFactor = lights[i].attenFactors;
            float distance = length(lights[i].pos.xyz - inPos);
            attenuation = 1.0f / (attenFactor[0] + attenFactor[1]*distance + attenFactor[2]*(distance*distance));
        }

        vec3 radiance = lights[i].color.rgb * lights[i].intensity * attenuation;

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
        vec3 R = reflect(-V, N);
        vec3 skyboxColor = texture(skybox, R).rgb;

        //vec3 specular = ( numerator / max(denom, 0.001f) ) * skyboxColor;
        vec3 specular = ( numerator / max(denom, 0.001f) );

        float NdotL = max(dot(N, L), 0.0f);
        Lo += (kD * albedo/PI + specular) * radiance * NdotL;
    }

    vec3 ambient = vec3(0.03f) * albedo* ao;
    vec3 color = ambient + Lo;

    float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));
    if (brightness > 1.0f)
        brightColor = vec4(color, 1.0f);
    else
        brightColor = vec4(vec3(0.0f), 1.0f);

    color = color / (color + vec3(1.0f));
    // apply gamma correction
    color = pow(color, vec3(1.0f/2.2f));

    fragColor = vec4(color, 1.0f);
}
