#version 460

in (layout location = 0) vec3 aPos;
in (layout location = 1) vec3 aColor;

out (layout location = 1) vec3 outColor;

void main()
{
    gl_Position = vec4(aPos, 1.0f);
}
