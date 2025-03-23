#pragma once

#include "shader.hpp"

#include <glad/glad.h>  // OpenGl library
#include <glm/gtc/matrix_transform.hpp>

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
        void Unuse();

        GLuint GetHandle();

        // utility uniform functions
        void SetBool(const std::string& name, bool value) const;
        void SetInt(const std::string& name, int value) const;
        void SetUnsigned(const std::string& name, unsigned int value) const;
        void SetFloat(const std::string& name, float value) const;
        void SetVec2(const std::string& name, glm::vec2 value) const;
        void SetVec3(const std::string& name, glm::vec3 value) const;
        void SetVec4(const std::string& name, glm::vec4 value) const;
        void SetMat4(const char* name, glm::mat4 value) const;

    private:
        GLuint  mHandleID = 0;            // program handle
        Shader* mVertShader = nullptr;
        Shader* mFragShader = nullptr;
    };


}