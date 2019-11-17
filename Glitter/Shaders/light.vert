#version 450 core

// ==============================================
layout (location = 0) in vec3 aPos; 
layout (location = 1) in vec2 aTexCoords;

layout (std140) uniform Matrices
{
    mat4 view;
    mat4 projection;
};

// ==============================================
out vec2 texCoords; 

// ==============================================
uniform mat4 model; 

// ==============================================
void main() 
{
    gl_Position = projection * view * model * vec4(aPos, 1.0f);
    texCoords = aTexCoords;   
}
