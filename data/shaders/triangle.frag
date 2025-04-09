#version 460

in vec3 frag_pos;
in vec3 frag_normal;
in vec2 frag_uv;


uniform vec2 uniform_heightMapRes;
uniform vec4 uniform_color;// vec3 color. If alpha 0, it's from the river else, patches
out vec4 frag_color;

void main()
{
    if(uniform_color.a == 1)
    {
        float col = clamp(frag_pos.y / (0.25 * uniform_heightMapRes.x), 0.0, 1.0);

        frag_color = vec4(vec3(col, 0, 0), 1.0);
    }
    else
    {
        frag_color = uniform_color;
    }
}