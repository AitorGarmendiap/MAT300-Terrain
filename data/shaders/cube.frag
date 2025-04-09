#version 460

in vec3 frag_pos;
in vec3 frag_normal;
in vec2 frag_uv;

uniform vec3 uniform_Color;

out vec4 frag_color;

void main()
{
    frag_color = vec4(uniform_Color, 1.0);
}