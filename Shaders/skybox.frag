#version 450 core

// =========================================
layout (location = 0) in vec3 TexCoords;

// =========================================
out vec4 fragColor;

// =========================================
uniform samplerCube skybox;

// =========================================
void main()
{
    fragColor = texture(skybox, TexCoords);
}
