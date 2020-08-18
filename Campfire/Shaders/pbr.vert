#version 450 core

// =========================================
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV;
layout (location = 2) in vec3 aNormal;

// =========================================
layout (location = 0) out vec3 outPos;
layout (location = 1) out vec2 outUV;
layout (location = 2) out vec3 outNormal;
layout (location = 3) out vec3 outCamPos;

// =========================================
layout (std140, binding = 0) uniform Camera
{
    mat4 view;
    mat4 proj;
    mat4 viewProj;

    vec4 pos;
};

// =========================================
uniform mat4 model;

// =========================================
void main()
{
    gl_Position = viewProj * model * vec4(aPos, 1.0f);
    outPos = vec3(model*vec4(aPos, 1.0f));
    outUV = aUV;
    outCamPos = pos.rgb;

    // TODO find a way to not do this too often
    // pass as uniform maybe?
    outNormal = mat3(transpose(inverse(model))) * aNormal;
}
