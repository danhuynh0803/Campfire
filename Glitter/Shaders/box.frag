#version 450 core

struct Light
{
    vec3 pos;
    vec3 color;
};

in vec3 position;
in vec2 texCoords;
in vec3 normals;

out vec4 fragColor;

uniform sampler2D tex;
uniform Light lights[1];

vec4 Phong()
{
    vec3 color = texture(tex, texCoords).rgb;

    float ambient = 0.2f;
    float diffuse = 0.0f;
    float specular = 0.0f;

    for (int i = 0; i < lights.length; ++i)
    {
        // Diffuse portion
        //vec3 dir = normalize(
    }

    //return vec4(ambient * color, 1.0f); //+ diffuse + specular;
    return vec4(normals, 1.0f);
}

void main()
{
    //fragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
    fragColor = Phong();
}
