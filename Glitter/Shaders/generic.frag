#version 450 core

// =========================================
const int NUM_LIGHTS = 5;

// =========================================
struct Light
{
    vec4 pos;
    vec4 color;
};

// =========================================
layout (std140, binding = 1) uniform LightBuffer
{
    Light lights[NUM_LIGHTS];
};

// =========================================
in vec3 position;
in vec2 uvCoords;
in vec3 normal;

// =========================================
out vec4 fragColor;

// =========================================
uniform sampler2D texIn;

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

// =========================================
vec3 Phong()
{
    vec3 albedo = texture(texIn, uvCoords).rgb;
    float ambient = 0.2f;
    float specularCoeff = 0.0f;
    vec3 diffuse = vec3(0.0f);
    vec3 specular = vec3(0.0f);

    for (int i = 0; i < lights.length(); ++i)
    {
        vec3 Li = normalize(lights[i].pos.xyz - position);
        // Diffuse portion
        diffuse += max(0.0f, dot(Li, normal)) * lights[i].color.rgb;
    }

    vec3 totalColor = (ambient + diffuse + specular) * albedo;

    return totalColor;
}

// =========================================
void main()
{
    fragColor = vec4(Phong(), 1.0f);
}
