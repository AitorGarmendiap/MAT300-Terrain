#include "shader_program.hpp"

namespace mat300_terrain{

    ShaderProgram::ShaderProgram()
    {
    }
    ShaderProgram::~ShaderProgram()
    {
    }
    void ShaderProgram::AttachVertShader(Shader& shader)
    {
    }
    void ShaderProgram::AttachFragShader(Shader& shader)
    {
    }
    void ShaderProgram::LinkProgram()
    {
    }
    void ShaderProgram::Use()
    {
    }
    GLuint ShaderProgram::GetHandle()
    {
        return mID;
    }
}