#include "shader_program.hpp"

#include <iostream>
namespace mat300_terrain{

    ShaderProgram::ShaderProgram()
    {
    }
    ShaderProgram::~ShaderProgram()
    {
        glUseProgram(0);

        if (mVertShader != NULL)
        {
            glDetachShader(mHandleID, mVertShader->GetID());
        }
        if (mFragShader != NULL)
        {
            glDetachShader(mHandleID, mFragShader->GetID());
        }

        glDeleteProgram(mHandleID);
    }
    void ShaderProgram::CreateProgram()
    {
        mHandleID = glCreateProgram();

        if (mHandleID == 0)
        {
            throw std::runtime_error("Could not create shader program");
        }
    }

    void ShaderProgram::AttachVertShader(Shader& shader)
    {
        // Detach previosly attached shader
        if (mVertShader != nullptr)
        {
            glDetachShader(mHandleID, mVertShader->GetID());
        }

        mVertShader = &shader;
        glAttachShader(mHandleID, shader.GetID());
    }
    void ShaderProgram::AttachFragShader(Shader& shader)
    {
        // Detach previosly attached shader
        if (mFragShader != nullptr)
        {
            glDetachShader(mHandleID, mFragShader->GetID());
        }

        mFragShader = &shader;
        glAttachShader(mHandleID, shader.GetID());
    }
    void ShaderProgram::LinkProgram()
    {
        glLinkProgram(mHandleID);

        GLint success;
        char infoLog[512];
        glGetProgramiv(mHandleID, GL_LINK_STATUS, &success);

        // Check if shader program was succesfully linked
        if (!success)
        {
            glGetProgramInfoLog(mHandleID, 1024, NULL, infoLog);
            std::cout << "Failed to link Program: " << " Error:\n" << infoLog << std::endl;
            throw std::runtime_error("Could not link shader program");
        }

    }
    void ShaderProgram::Use()
    {
        glUseProgram(mHandleID);
    }
    GLuint ShaderProgram::GetHandle()
    {
        return mHandleID;
    }
}