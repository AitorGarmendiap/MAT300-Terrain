#pragma once

#include "shader.hpp"

namespace mat300_terrain {

    class ShaderProgram
    {
    public:
        ShaderProgram();
        ~ShaderProgram();

        void AttachVertShader(Shader& shader);
        void AttachFragShader(Shader& shader);
        void LinkProgram();
        void Use();

        GLuint GetHandle();

    private:
        GLuint  mID;         // program handle
        Shader* mVertShader = nullptr;
        Shader* mFragShader = nullptr;
    };


}