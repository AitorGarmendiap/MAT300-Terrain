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
    void ShaderProgram::Unuse()
    {
        glUseProgram(0);
    }
    GLuint ShaderProgram::GetHandle()
    {
        return mHandleID;
    }
    // utility uniform functions
// ------------------------------------------------------------------------
    void ShaderProgram::SetBool(const std::string& name, bool value) const
    {
        GLint location = glGetUniformLocation(mHandleID, name.c_str());
        if (location == -1)
        {

            std::cout << "Failed to correctly retrieve location for bool, name: " << name << std::endl;
        }
        else
        {
            glUniform1i(location, value);
        }
    }
    // ------------------------------------------------------------------------
    void ShaderProgram::SetInt(const std::string& name, int value) const
    {
        GLint location = glGetUniformLocation(mHandleID, name.c_str());
        if (location == -1)
        {
            std::cout << "Failed to correctly retrieve location for int, name: " << name << std::endl;
        }
        else
        {
            glUniform1i(location, value);
        }
    }
    void ShaderProgram::SetUnsigned(const std::string& name, unsigned int value) const
    {
        GLint location = glGetUniformLocation(mHandleID, name.c_str());
        if (location == -1)
        {
            std::cout << "Failed to correctly retrieve location for Unsigned int, name: " << name << std::endl;
        }
        else
        {
            glUniform1ui(location, value);
        }
    }
    // ------------------------------------------------------------------------
    void ShaderProgram::SetFloat(const std::string& name, float value) const
    {
        GLint location = glGetUniformLocation(mHandleID, name.c_str());
        if (location == -1)
        {
            std::cout << "Failed to correctly retrieve location for float, name: " << name << std::endl;
        }
        else
        {
            glUniform1f(location, value);
        }
    }
    /**
    * @brief
    *  uploads a vec2 to the shader
    * @param name
    *   name of the uniform
    * @param value
    *   value of the uniform
    */
    void ShaderProgram::SetVec2(const std::string& name, glm::vec2 value) const
    {
        GLint location = glGetUniformLocation(mHandleID, name.c_str());

        if (location == -1)
        {
            std::cout << "Failed to correctly retrieve location for vec2, name: " << name << std::endl;
        }
        else
        {
            glUniform2fv(location, 1, reinterpret_cast<const GLfloat*>(&value));
        }
    }
    /**
    * @brief
    *  uploads a vec3 to the shader
    * @param name
    *   name of the uniform
    * @param value
    *   value of the uniform
    */
    void ShaderProgram::SetVec3(const std::string& name, glm::vec3 value) const
    {
        GLint location = glGetUniformLocation(mHandleID, name.c_str());

        if (location == -1)
        {
            std::cout << "Failed to correctly retrieve location for vec3, name: " << name << std::endl;
        }
        else
        {
            glUniform3fv(location, 1, reinterpret_cast<const GLfloat*>(&value));
        }
    }
    /**
    * @brief
    *  uploads a vec4 to the shader
    * @param name
    *   name of the uniform
    * @param value
    *   value of the uniform
    */
    void ShaderProgram::SetVec4(const std::string& name, glm::vec4 value) const
    {
        GLint location = glGetUniformLocation(mHandleID, name.c_str());

        if (location == -1)
        {
            std::cout << "Failed to correctly retrieve location for vec4, name: " << name << std::endl;
        }

        glUniform4fv(location, 1, reinterpret_cast<const GLfloat*>(&value));
    }
    /**
    * @brief
    *  uploads a matrix 4x4 to the shader
    * @param name
    *   name of the uniform
    * @param value
    *   value of the uniform
    */
    void ShaderProgram::SetMat4(const char* name, glm::mat4 value) const
    {

        GLint location = glGetUniformLocation(mHandleID, name);

        if (location == -1)
        {
            std::cout << "Failed to correctly retrieve location for mat4, name: " << name << std::endl;
        }

        glUniformMatrix4fv(location, 1, GL_FALSE, reinterpret_cast<const GLfloat*>(&value));
    }
}