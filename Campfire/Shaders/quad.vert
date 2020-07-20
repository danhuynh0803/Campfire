#version 460

// =========================================
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec2 aUV;
layout (location = 3) in float aTexIndex;


// =========================================
layout (location = 0) out vec4 outColor;
layout (location = 1) out vec2 outUV;
layout (location = 2) out float outTexIndex;

// =========================================
layout (std140, binding = 0) uniform Camera
{
    mat4 view;
    mat4 proj;
    mat4 viewProj;
};

// =========================================
uniform mat4 model;

// =========================================
void main()
{
    outColor = aColor;
    outUV = aUV;
    outTexIndex = aTexIndex;

    gl_Position =
        viewProj
        * model
        * vec4(aPos, 1.0f)
    ;
}
