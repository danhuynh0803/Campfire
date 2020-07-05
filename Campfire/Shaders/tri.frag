#version 460

layout (location = 0) in vec2 inUV;

out vec4 fragColor;

uniform vec4 color;
uniform float time;

uniform sampler2D texIn;

float Random(float t)
{
    return fract(sin(t) * 1e4);
}

float Random(vec2 uv)
{
    return fract(sin(dot(uv, vec2(12.313, 53.34))) * 100000.0f);
}

float Threshold(vec3 col, float threshold)
{
    if (col.r < threshold)
    {
        return 0.0f;
    }
    return 1.0f;
}

void main()
{
//    vec2 uv = gl_FragCoord.xy / vec2(1600.0f, 900.0f);
//
    vec2 uv = inUV;
    vec2 grid = vec2(100.0f, 100.0f);
    vec2 ipos = uv * grid;

    vec2 fpos = fract(uv * grid);

    float shiftFactor = time * 10.0f * (Random(floor(ipos.y+1))*0.5f + 0.5f);

    ipos.x -= shiftFactor;

    ipos = floor(ipos);

    vec3 col = vec3(Random(ipos));
    col = vec3(Threshold(col, 0.3f));
    col *= step(0.25f, fpos.y);

    vec3 texColor = texture(texIn, inUV).rgb;
    fragColor = vec4(texColor * col * vec3(uv, 0.0f), 1.0f);

    //fragColor = texture(texIn, inUV);
}
