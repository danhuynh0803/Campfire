#version 460 core

in vec2 texCoords;

out vec4 fragColor;

uniform sampler2D screenTex;

void main()
{
    fragColor = texture(screenTex, texCoords);
    // Invert test
    fragColor.rgb = vec3(1.0f) - fragColor.rgb;
}
