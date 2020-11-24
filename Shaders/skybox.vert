#version 450 core

// =========================================
layout (location = 0) in vec3 aPos;

// =========================================
layout (location = 0) out vec3 TexCoords;

// =========================================
layout (std140, binding = 0) uniform Camera
{
    mat4 view;
    mat4 proj;
    mat4 viewProj;
};


void main()
{
    TexCoords = aPos;
    gl_Position = proj * mat4(mat3(view)) * vec4(aPos, 1.0f);
}
