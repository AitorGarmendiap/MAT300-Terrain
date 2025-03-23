#version 460
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 frag_pos;
out vec3 frag_Normal;       // Normal to be interpolated

uniform mat4 uniform_Model;
uniform mat4 uniform_View;
uniform mat4 uniform_Proj;

void main()
{
    frag_pos = vec3(uniform_View * uniform_Model * vec4(aPos, 1.0));                // Transform position to world space
    frag_Normal = mat3(transpose(inverse(uniform_Model))) * aNormal; // Correct normal transformation

    gl_Position = uniform_Proj * vec4(frag_pos, 1.0);
}