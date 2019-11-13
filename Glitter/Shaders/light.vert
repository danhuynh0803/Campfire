#version 450 core

// ==============================================
layout (location = 0) in vec3 aPos; 
layout (location = 1) in vec2 aTexCoords;

// ==============================================
out vec2 texCoords; 

// ==============================================
//uniform mat4 model; 
//uniform mat4 view;
//uniform mat4 proj;
uniform mat4 mvp;

// ==============================================
void main() 
{
    gl_Position = mvp * vec4(aPos, 1.0f);
    texCoords = aTexCoords;   
}
