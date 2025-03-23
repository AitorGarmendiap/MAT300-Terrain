#pragma once

#include "shader.hpp"

namespace mat300_terrain {

    class ShaderProgram
    {
    public:
        ShaderProgram();
        ~ShaderProgram();

        void CreateProgram();
        void AttachVertShader(Shader& shader);
        void AttachFragShader(Shader& shader);
        void LinkProgram();
        void Use();

        GLuint GetHandle();

    private:
        GLuint  mHandleID = 0;            // program handle
        Shader* mVertShader = nullptr;
        Shader* mFragShader = nullptr;
    };


}