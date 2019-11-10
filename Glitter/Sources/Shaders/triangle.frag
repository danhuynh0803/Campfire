#version 450 core

in vec3 vertColor;
out vec4 fragColor; 

void main()
{
    //fragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
    fragColor = vec4(vertColor, 1.0f);
}