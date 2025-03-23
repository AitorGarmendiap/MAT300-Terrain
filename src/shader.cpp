#include "shader.hpp"

#include <fstream>
#include <iostream>

namespace mat300_terrain{

    Shader::Shader()
    {
    }

    Shader::~Shader()
    {

    }

    std::string LoadShaderSource(const char* fileName)
    {
        std::string temp = "";
        std::string src = "";

        std::ifstream inFile(fileName);

        if (inFile.is_open())
        {
            while (std::getline(inFile, temp))
            {
                src += temp + "\n";
            }
        }
        else
        {
            std::cout << "ERROR: Could not open Shader file: " << fileName << std::endl;
            throw std::runtime_error("Could not open Shader file");
        }
        inFile.close();

        return src;
    }
    void Shader::CreateShader(GLenum shaderType, std::string name)
    {
        mName = name;
        mType = shaderType;

        mShaderID = glCreateShader(shaderType);

        if (mShaderID == 0)
        {
            throw std::runtime_error("Could not create shader");
        }

        std::string shader_src = LoadShaderSource(mName.c_str());

        const GLchar* src = shader_src.c_str();

        glShaderSource(mShaderID, 1, &src, NULL);
    }

    void Shader::CompileShader()
    {
        glCompileShader(mShaderID);

        GLint success;
        char infoLog[512];
        glGetShaderiv(mShaderID, GL_COMPILE_STATUS, &success);

        if (!success)
        {
            glGetShaderInfoLog(mShaderID, 512, NULL, infoLog);
            std::cout << "Failed to compile shader:" << mName << " Error:\n" << infoLog << std::endl;
            throw std::runtime_error("Could not compile shader program");
        }
    }

    GLuint Shader::GetID()
    {
        return mShaderID;
    }
}