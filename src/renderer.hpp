#pragma once
#include "camera.hpp"
#include "patches.hpp"
#include "shader.hpp"
#include "shader_program.hpp"
#include "river.hpp"

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

        void DrawRiver(glm::vec3 pos, glm::vec3 scale, glm::vec3 forward, glm::vec3 normal);
        void DrawCube(glm::vec3 pos, float scale = 1);
        void DrawTriangles(const std::vector<glm::vec3>& triangles);
        void DrawLines(const std::vector<glm::vec3>& lines);

        void Update(const Camera& cam, const std::vector<Patch>& patches, const River& river, int divCount);
        std::vector<glm::vec3> TriangulateMesh(const Patch& patch);
        std::vector<glm::vec3> LineMesh(const std::vector<glm::vec3>& line);

        int SelectedPatch = -1;
        int SelectedPoint = -1;
        bool wireframe = true;
        bool drawControlPoints = true;

        glm::vec3 patchColor = { 0.2, 0.6, 0.5 };
        glm::vec3 borderColor = { 1.0, 1.0, 0.0 };
        glm::vec3 selectedColor = { 0.0, 1.0, 0.0 };
        
    private:

        ShaderProgram mSimpleShaderProg;
        Shader        mSimpleVertShader;
        Shader        mSimpleFragShader;
        ShaderProgram mTriangleShaderProg;
        Shader        mTriangleVertShader;
        Shader        mTriangleFragShader;
        ShaderProgram mLineShaderProg;
        Shader        mLineVertShader;
        Shader        mLineFragShader;

        // Cube stuff
        void CreateTriangleArray();
        void ReCreateTriangleArray(const std::vector<glm::vec3>& triangles);
        void ReCreateLineArray(const std::vector<glm::vec3>& lines);
        void CreateCube();
        void CreateLine();

        GLuint mVAOtr;
        GLuint mVBOtr;
        GLuint mEBOtr;
        GLuint mVAOln;
        GLuint mVBOln;
        GLuint mEBOln;
        GLuint mVAO;
        GLuint mVBO;
        GLuint mEBO;

    };
}