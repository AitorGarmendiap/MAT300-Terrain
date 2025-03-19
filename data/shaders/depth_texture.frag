#version 460

in vec2 frag_uv;

uniform sampler2D depthTex;
uniform float near_plane;
uniform float far_plane;

out vec4 frag_color;

float linearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC [-1, 1] 
    return ((2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane))) / far_plane;
}

void main()
{
    float depth = texture(depthTex, frag_uv).x;

    frag_color = vec4(vec3(linearizeDepth(depth)), 1.0f);
}