#version 450 core

// ==============================================
out vec4 fragColor; 

// ==============================================
in vec2 texCoords; 

// ==============================================
uniform sampler2D tex1;
uniform sampler2D tex2;
uniform float texMix;

void main()
{
    //fragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);    
    //fragColor = vec4(texCoords, 0.0f, 1.0f);
    //fragColor = mix(texture(tex1, texCoords), texture(tex2, texCoords), texMix);
    fragColor = texture(tex1, texCoords);
}
