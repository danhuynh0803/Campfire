#version 450 core

struct Light
{
    vec3 pos;
    vec3 color;
};

in vec3 position;
in vec2 texCoords;
in vec3 normal;

out vec4 fragColor;

uniform sampler2D tex;
uniform Light lights[5];

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
    //fragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
    fragColor = vec4(Phong(), 1.0f);
    //fragColor = vec4(normal, 1.0f);
}
