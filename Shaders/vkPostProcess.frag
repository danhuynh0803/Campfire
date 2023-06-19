#version 450 core
#extension GL_ARB_separate_shader_objects : enable

// =========================================
layout (location = 0) in vec2 inUV;

// =========================================
layout (location = 0) out vec4 outColor;

// =========================================
layout (set = 0, binding = 0) uniform sampler2D uInputTexture;

// =========================================
void main()
{
    vec3 col = texture(uInputTexture, inUV).rgb;
    vec3 newCol = vec3(1.0f) - col;

    outColor = vec4(newCol, 1.0f);
}
