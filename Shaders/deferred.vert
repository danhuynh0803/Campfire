#version 450

// =========================================
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV;
layout (location = 2) in vec3 aNormal;

// =========================================
layout (location = 0) out vec3 outPos;
layout (location = 1) out vec2 outUV;
layout (location = 2) out vec3 outNormal;

// =========================================
layout (set = 0, binding = 0) uniform Camera
{
    mat4 view;
    mat4 proj;
    mat4 viewProj;
    vec3 pos;
} camera;

layout (push_constant) uniform PushConstants {
    mat4 model;
} pushConstants;

// =========================================
void main()
{
    gl_Position = camera.viewProj * pushConstants.model * vec4(aPos, 1.0);

    outPos = vec3(pushConstants.model * vec4(aPos, 1.0f));
    outUV = vec2(aUV.x, 1.0f - aUV.y);
    outNormal = mat3(transpose(inverse(pushConstants.model))) * aNormal;
}
