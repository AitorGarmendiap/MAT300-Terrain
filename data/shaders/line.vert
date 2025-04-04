#version 460
layout (location = 0) in vec3 aPos;

uniform mat4 uniform_Model;
uniform mat4 uniform_View;
uniform mat4 uniform_Proj;

void main()
{
    gl_Position = uniform_Proj * uniform_View * uniform_Model * vec4(aPos, 1.0);
}