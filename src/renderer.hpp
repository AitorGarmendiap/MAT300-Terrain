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
        void DrawTriangles(const std::vector<glm::vec3>& triangles);

        void Update(float dt, const Camera& cam, const std::vector<Patch>& patches);
        std::vector<glm::vec3> TriangulateMesh(const Patch& patch);

        int SelectedPatch = -1;
        int SelectedPoint = -1;

        glm::vec3 patchColor = { 1.0, 1.0, 0.0 };
        glm::vec3 borderColor = { 1.0, 1.0, 1.0 };
        glm::vec3 selectedColor = { 1.0, 0.0, 0.0 };
        
    private:

        ShaderProgram mSimpleShaderProg;
        Shader        mSimpleVertShader;
        Shader        mSimpleFragShader;
        ShaderProgram mTriangleShaderProg;
        Shader        mTriangleVertShader;
        Shader        mTriangleFragShader;

        // Cube stuff
        void CreateTriangleArray();
        void ReCreateTriangleArray(const std::vector<glm::vec3>& triangles);

        GLuint mVAOtr;
        GLuint mVBOtr;
        GLuint mEBOtr;
        void CreateCube();
        GLuint mVAO;
        GLuint mVBO;
        GLuint mEBO;

    };
}