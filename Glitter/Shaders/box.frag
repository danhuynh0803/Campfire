#version 450 core

const int NUM_LIGHTS = 5;

struct Light
{
    vec3 pos;
    vec3 color;
};

// =========================================
//layout (std140) uniform LightBlock
//{
//    Light lights[NUM_LIGHTS];
//};

// =========================================
in vec3 position;
in vec2 texCoords;
in vec3 normal;

// =========================================
out vec4 fragColor;

// =========================================
uniform Light lights[NUM_LIGHTS];
uniform sampler2D tex;

float near = 0.1f;
float far = 100.0f;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0f - 1.0f;
    return (2.0f * near * far) / (far + near - z * (far - near));
}

vec3 Phong()
{
    vec3 albedo = texture(tex, texCoords).rgb;
    //albedo = vec3(0.3f);

    float ambient = 0.3f;
    float specularCoeff = 0.0f;

    vec3 diffuse = vec3(0.0f);
    vec3 specular = vec3(0.0f);

    for (int i = 0; i < lights.length; ++i)
    {
        vec3 Li = normalize(lights[i].pos - position);
        // Diffuse portion
        diffuse += max(0.0f, dot(Li, normal)) * lights[i].color;
    }


    vec3 totalColor = (ambient + diffuse + specular) * albedo;

    return totalColor;
    //return lights[0].color;
}

void main()
{
    fragColor = vec4(Phong(), 1.0f);
    float depth = LinearizeDepth(gl_FragCoord.z) / far;
    fragColor = vec4(vec3(1.0f - depth), 1.0f);
}
