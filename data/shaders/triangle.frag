#version 460

in vec3 frag_pos;
in vec3 frag_normal;
in vec2 frag_uv;


uniform vec2 uniform_heightMapRes;

out vec4 frag_color;

void main()
{
    float col = clamp(frag_pos.y / (0.25 * uniform_heightMapRes.x), 0.0, 1.0);

    frag_color = vec4(vec3(col, 0, 0), 1.0);
}