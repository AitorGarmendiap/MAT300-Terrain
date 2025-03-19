#version 460
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aUV;

out vec2 frag_uv;

void main()
{
    frag_uv = aUV;
    gl_Position = vec4(aPosition, 1.0f);
}