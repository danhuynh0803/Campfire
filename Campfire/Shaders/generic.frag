#version 450 core

// =========================================
const int MAX_NUM_LIGHTS = 25;

// =========================================
struct Light
{
    vec4 pos;
    vec4 color;

    //vec4 ambient;
    //vec4 diffuse;
    //vec4 specular;

    // packed into a vec4
    //x: constant
    //y: linear
    //z: quadratic
    //w: padding
    vec4 attenFactors;
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

// =========================================

//float near = 0.1f;
//float far = 100.0f;

// =========================================
// TODO for shadows
//float LinearizeDepth(float depth)
//{
//    float z = depth * 2.0f - 1.0f;
//    return (2.0f * near * far) / (far + near - z * (far - near));
//}

//vec3 CalcPointLight()
//{
//
//}

// =========================================
vec3 Phong()
{
    vec4 color = texture(texDiffuse, uvCoords);
    if (color.a < 0.01) { discard; }

    vec3 albedo = color.rgb;

    float specularCoeff = 0.0f;
    vec3 diffuse = vec3(0.0f);
    vec3 specular = vec3(0.0f);

    vec3 ambient = 0.1f * albedo;

    vec3 totalColor = vec3(0.0f);

    for (int i = 0; i < numLights; ++i)
    {
        vec4 attenFactor = lights[i].attenFactors;
        float distance = length(lights[i].pos.xyz - position);
        float attenuation = 1.0f / (attenFactor[0] + attenFactor[1]*distance + attenFactor[2]*(distance*distance));

        ambient *= attenuation;

        // Diffuse portion
        vec3 Li = normalize(lights[i].pos.xyz - position);
        diffuse = max(0.0f, dot(Li, normal)) * lights[i].color.rgb * attenuation;

        // TODO specular with spec maps

        totalColor += ambient + (diffuse + specular) * albedo;
    }

    return totalColor;
}

// =========================================
void main()
{
    //fragColor = vec4(Phong(), 1.0f);
    vec4 color = texture(texDiffuse, uvCoords);
    //color = vec4(1.0f);
    //color.rgb = Phong();
    //if (color.a < 0.01) { discard; }
    fragColor = color;
}
