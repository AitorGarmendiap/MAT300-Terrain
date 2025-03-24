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

        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glCullFace(GL_BACK);
    }

    void Renderer::Update(float dt, const Camera& cam, const std::vector<Patch>& patches)
    {
        mSimpleShaderProg.Use();

        mSimpleShaderProg.SetMat4("uniform_View", cam.GetView());
        mSimpleShaderProg.SetMat4("uniform_Proj", cam.GetProjection());

        mSimpleShaderProg.SetVec3("lightPos", glm::vec3(5.0f, 5.0f, 5.0f));
        mSimpleShaderProg.SetVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f)); // White light
        mSimpleShaderProg.SetVec3("viewPos", cam.GetPosition());

        float scale = 5;

        mSimpleShaderProg.SetVec3("uniform_Color", glm::vec3(1.0, 1.0, 0.0));
        //DrawCube(glm::vec3(0.0, 0.0, -5.0), 5);

        for (const auto& patch : patches)
        {
            mSimpleShaderProg.SetVec3("uniform_Color", glm::vec3(1.0, 1.0, 0.0));
           
            for (const auto& cPoint : patch.controlPoints)
            {
                float scale = 0.1;
                for (const auto& pt : cPoint)
                {
                    DrawCube(pt, scale);
                }
            }

            for (const auto& meshPoint : patch.mesh)
            {
                float scale = 0.01;

                DrawCube(meshPoint, scale);
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
            glm::vec2 uv;
        };
        Vertex cubeVertices[] =
        {
            // Front face
            {{+0.5f, +0.5f, +0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}}, // Top Right
            {{+0.5f, -0.5f, +0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}}, // Bottom Right
            {{-0.5f, +0.5f, +0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}}, // Top Left
            {{-0.5f, -0.5f, +0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}}, // Bottom Left

            // Back face
            {{+0.5f, +0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}}, // Top Right
            {{+0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}}, // Bottom Right
            {{-0.5f, +0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}}, // Top Left
            {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}}, // Bottom Left

            // Right face
            {{+0.5f, +0.5f, +0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}}, // Top Front
            {{+0.5f, -0.5f, +0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}}, // Bottom Front
            {{+0.5f, +0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}}, // Top Back
            {{+0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}}, // Bottom Back

            // Left face
            {{-0.5f, +0.5f, +0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}}, // Top Front
            {{-0.5f, -0.5f, +0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}}, // Bottom Front
            {{-0.5f, +0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}}, // Top Back
            {{-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}}, // Bottom Back

            // Top face
            {{+0.5f, +0.5f, +0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}}, // Front Right
            {{+0.5f, +0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}}, // Back Right
            {{-0.5f, +0.5f, +0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}}, // Front Left
            {{-0.5f, +0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}}, // Back Left

            // Bottom face
            {{+0.5f, -0.5f, +0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}}, // Front Right
            {{+0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}}, // Back Right
            {{-0.5f, -0.5f, +0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}}, // Front Left
            {{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}}, // Back Left
        };

        GLuint cubeIndices[] =
        {
            
            0, 2, 3, 0, 3, 1,       // Front
            4, 5, 7, 4, 7, 6,       // Back
            8, 9, 11, 8, 11, 10,    // Right
            13, 12, 15, 12, 14, 15, // Left
            16, 17, 19, 16, 19, 18, // Top
            21, 20, 23, 20, 22, 23, // Bottom
        };
        GLuint cubeIndexCount = sizeof(cubeIndices) / sizeof(GLuint);


        glGenVertexArrays(1, &mVAO);
        glGenBuffers(1, &mVBO);
        glGenBuffers(1, &mEBO);

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

        glEnableVertexAttribArray(2); // Normal
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));

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