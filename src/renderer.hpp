#pragma once
#include "camera.hpp"
#include "patches.hpp"
#include "shader.hpp"
#include "shader_program.hpp"

#include <glad/glad.h>  // OpenGl library

#include <vector>
#include <glm/glm.hpp>
namespace mat300_terrain {



    class Renderer
    {
    public:
        Renderer();
        ~Renderer() {}

        Renderer(const Renderer&) = delete;
        Renderer& operator=(const Renderer&) = delete;

        void DrawCube(glm::vec3 pos, float scale = 1);
        void Update(float dt, const Camera& cam, const std::vector<Patch>& patches);
        std::vector<glm::vec3> TriangulateMesh(const Patch& patch);

    private:

        ShaderProgram mSimpleShaderProg;
        Shader        mSimpleVertShader;
        Shader        mSimpleFragShader;

        // Cube stuff
        void CreateCube();
        GLuint mVAO;
        GLuint mVBO;
        GLuint mEBO;

    };
}