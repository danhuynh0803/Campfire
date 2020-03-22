#version 450 core

// =========================================
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aNormal;

// =========================================
layout (std140, binding = 0) uniform Matrices
{
    mat4 view;
    mat4 projection;
};

// =========================================
out vec3 position;
out vec2 uvCoords;
out vec3 normal;

// =========================================
uniform mat4 model;

// =========================================
void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0f);
    position = vec3(model*vec4(aPos, 1.0f));
    uvCoords = aTexCoords;
    normal = aNormal;
}
