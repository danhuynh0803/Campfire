#version 450 core

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
};

// =========================================
layout (std140, binding = 1) uniform LightBuffer
{
    Light lights[MAX_NUM_LIGHTS];
    uint numLights;
};

// =========================================
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uvCoords;
layout (location = 2) in vec3 normal;

// =========================================
out vec4 fragColor;

// =========================================
uniform sampler2D texDiffuse;
uniform sampler2D texSpecular;
uniform sampler2D texNormals;
uniform sampler2D texBump;

// =========================================
vec4 Phong()
{
    vec4 color = texture(texDiffuse, uvCoords);
    if (color.a < 0.01) { discard; }

    // Albedo
    vec3 albedo = color.rgb;
    vec3 ambient = 0.1f * albedo;

    // Diffuse
    vec3 diffuse = vec3(0.0f);

    // Specular
    float specularCoeff = 0.0f;
    vec3 specular = vec3(0.0f);

    vec3 totalColor = vec3(0.0f);
    for (int i = 0; i < numLights; ++i)
    {
        vec4 attenFactor = lights[i].attenFactors;

        float attenuation;
        uint type = uint(round(attenFactor.w));
        if (type == 0)
        {
            vec3 lightDir = -1.0f * normalize(lights[i].lightDir.rgb);
            diffuse = max(0.0f, dot(lightDir, normal)) * lights[i].color.rgb;
        }
        else if (type == 1)
        {
            float distance = length(lights[i].pos.xyz - position);
            attenuation = 1.0f / (attenFactor[0] + attenFactor[1]*distance + attenFactor[2]*(distance*distance));

            // Diffuse portion
            vec3 Li = normalize(lights[i].pos.xyz - position);
            diffuse = max(0.0f, dot(Li, normal)) * lights[i].color.rgb * attenuation;

            ambient *= attenuation;
        }

        // TODO specular with spec maps
        totalColor += ambient + (diffuse + specular) * albedo;
    }

    return vec4(totalColor, 1.0f);
}

// =========================================
void main()
{
    vec4 color = Phong();
    fragColor = color;
}
