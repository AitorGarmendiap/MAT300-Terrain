#include "renderer.hpp"

namespace mat300_terrain {

    Renderer::Renderer()
    {

        mSimpleShaderProg.CreateProgram();

        // shaders for drawing shapes
        mSimpleVertShader.CreateShader(GL_VERTEX_SHADER, "data/shaders/cube.vert");
        mSimpleFragShader.CreateShader(GL_FRAGMENT_SHADER, "data/shaders/cube.frag");

        mSimpleVertShader.CompileShader();
        mSimpleFragShader.CompileShader();

        mSimpleShaderProg.AttachVertShader(mSimpleVertShader);
        mSimpleShaderProg.AttachFragShader(mSimpleFragShader);

        mSimpleShaderProg.LinkProgram();

        CreateCube();
    }

    void Renderer::Update(float dt, const Camera& cam, const std::vector<Patches>& patches)
    {
        mSimpleShaderProg.Use();

        mSimpleShaderProg.SetMat4("uniform_View", cam.GetView());
        mSimpleShaderProg.SetMat4("uniform_Proj", cam.GetProjection());

        mSimpleShaderProg.SetVec3("lightPos", glm::vec3(5.0f, 5.0f, 5.0f));
        mSimpleShaderProg.SetVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f)); // White light
        mSimpleShaderProg.SetVec3("viewPos", cam.GetPosition());

        float scale = 5;

        DrawCube(glm::vec3(0.0, 0.0, -5.0), 5);

        for (const auto& patch : patches)
        {
            mSimpleShaderProg.SetVec3("uniform_Color", glm::vec3(1.0, 0.0, 0.0));

            for (const auto& cPoint : patch.controlPoints)
            {
                float scale = 5;

                DrawCube(cPoint, scale);

            }
        }
        mSimpleShaderProg.Unuse();
    }


    void Renderer::CreateCube()
    {
        struct Vertex
        {
            glm::vec3 position;
            glm::vec3 normal;
        };
        Vertex cubeVertices[] =
        {
            // 3D position  // Color

        {{+0.5f, +0.5f, +0.5f}, {0.0f, 0.0f, 1.0f}}, // Frot Top right
        {{+0.5f, -0.5f, +0.5f}, {0.0f, 0.0f, 1.0f}}, // Frot Bottom right
        {{-0.5f, +0.5f, +0.5f}, {0.0f, 0.0f, 1.0f}}, // Frot Top Left
        {{-0.5f, -0.5f, +0.5f}, {0.0f, 0.0f, 1.0f}}, // Frot Bottom Left

        {{+0.5f, +0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}},// Back Top right
        {{+0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}},// Back Bottom right
        {{-0.5f, +0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}},// Back Top Left
        {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}},// Back Bottom Left
        };

        GLuint cubeIndices[] =
        {
             0, 2, 3, 0, 3, 1,  //Front
             2, 6, 7, 2, 7, 3,
             6, 4, 5, 6, 5, 7,
             4, 0, 1, 4, 1, 5,
             0, 4, 6, 0, 6, 2,
             1, 7, 5, 1, 3, 7,
        };
        GLuint cubeIndexCount = sizeof(cubeIndices) / sizeof(GLuint);


        // 2. Generate buffers
        glGenVertexArrays(1, &mVAO);
        glGenBuffers(1, &mVBO);
        glGenBuffers(1, &mEBO);

        // 3. Bind VAO (store attribute state)
        glBindVertexArray(mVAO);

        // 4. Bind and upload vertex data
        glBindBuffer(GL_ARRAY_BUFFER, mVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

        // 5. Bind and upload index data
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);

        // 6. Define vertex attributes (position, color, texture, normal)
        glEnableVertexAttribArray(0); // Position
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

        glEnableVertexAttribArray(1); // Normal
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

        // 7. Unbind to prevent accidental changes
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);


    }
    void Renderer::DrawCube(glm::vec3 pos, float scale)
    {
        glBindVertexArray(mVAO);

        // Apply transformation
        glm::mat4 model = glm::translate(glm::mat4(1.0f), pos);
        model = glm::scale(model, glm::vec3(scale));

        mSimpleShaderProg.SetMat4("uniform_Model", model);

        // Draw cube
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);

    }
}