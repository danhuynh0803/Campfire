#version 450 core

in vec3 TexCoords;

out vec4 fragColor;

uniform samplerCube skybox;

void main()
{
    fragColor = texture(skybox, TexCoords);
}
