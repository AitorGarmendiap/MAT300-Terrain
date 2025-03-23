#pragma once

#include <glad/glad.h>  // OpenGl library
#include <GLFW/glfw3.h> // For contexts

#include <string>

namespace mat300_terrain {

    class Shader
    {
    public:
        Shader();
        ~Shader();

        void CreateShader(GLenum shaderType, std::string name);
        void CompileShader();

        GLuint GetID();

    private:

        std::string mName = "";
        GLenum  mType = 0;
        GLuint  mShaderID = 0;
    };


}