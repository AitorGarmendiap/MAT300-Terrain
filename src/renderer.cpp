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

        mTriangleShaderProg.CreateProgram();

        // shaders for drawing shapes
        mTriangleVertShader.CreateShader(GL_VERTEX_SHADER, "data/shaders/triangle.vert");
        mTriangleFragShader.CreateShader(GL_FRAGMENT_SHADER, "data/shaders/triangle.frag");

        mTriangleVertShader.CompileShader();
        mTriangleFragShader.CompileShader();

        mTriangleShaderProg.AttachVertShader(mTriangleVertShader);
        mTriangleShaderProg.AttachFragShader(mTriangleFragShader);

        mTriangleShaderProg.LinkProgram();

        mLineShaderProg.CreateProgram();

        // shaders for drawing lines
        mLineVertShader.CreateShader(GL_VERTEX_SHADER, "data/shaders/line.vert");
        mLineFragShader.CreateShader(GL_FRAGMENT_SHADER, "data/shaders/line.frag");

        mLineVertShader.CompileShader();
        mLineFragShader.CompileShader();

        mLineShaderProg.AttachVertShader(mLineVertShader);
        mLineShaderProg.AttachFragShader(mLineFragShader);

        mLineShaderProg.LinkProgram();

        CreateTriangleArray();
        CreateCube();
        CreateLine();

        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);
    }

    void Renderer::Update(const Camera& cam, const Terrain& terrain)
    {
        mSimpleShaderProg.Use();

        mSimpleShaderProg.SetMat4("uniform_View", cam.GetView());
        mSimpleShaderProg.SetMat4("uniform_Proj", cam.GetProjection());

        mSimpleShaderProg.Unuse();

        mTriangleShaderProg.Use();

        mTriangleShaderProg.SetMat4("uniform_View", cam.GetView());
        mTriangleShaderProg.SetMat4("uniform_Proj", cam.GetProjection());
        mTriangleShaderProg.SetVec2("uniform_heightMapRes", glm::vec2(terrain.mWidth, terrain.mHeight)); // TODO use actual res

        mTriangleShaderProg.Unuse();

        mLineShaderProg.Use();
        mLineShaderProg.SetMat4("uniform_View", cam.GetView());
        mLineShaderProg.SetMat4("uniform_Proj", cam.GetProjection());
        mLineShaderProg.SetMat4("uniform_Model", glm::mat4(1.0));
        mLineShaderProg.Unuse();

        for (int p = 0; p < terrain.mPatches.size(); ++p)
        {
            const Patch& patch = terrain.mPatches[p];

            mSimpleShaderProg.Use();

            // draw patches and control points
            for (int i = 0; i < 4; ++i)
            {
                for (int j = 0; j < 4; ++j)
                {
                    float scale = (10.f * 2) / terrain.mDivCount;
                    const auto& pt = patch.controlPoints[i][j];
                    // borders
                    if (i == 0 && j == 0 || i == 0 && j == 3 || i == 3 && j == 0 || i == 3 && j == 3)
                    {
                        mSimpleShaderProg.SetVec3("uniform_Color", borderColor);
                    }
                    else if (p == SelectedPatch)
                    {
                        if (i == 0 && j == 1 || i == 0 && j == 2 || i == 1 && j == 0 || i == 2 && j == 0 ||
                            i == 3 && j == 1 || i == 3 && j == 2 || i == 1 && j == 3 || i == 2 && j == 3)
                        {
                            mSimpleShaderProg.SetVec3("uniform_Color", patchColor2);
                        }
                        else
                        {
                            mSimpleShaderProg.SetVec3("uniform_Color", patchColor);
                        }
                    }
                    else if (drawControlPoints) // smaller control points if not selected patch
                    {                      
                        scale = 10.f / terrain.mDivCount;
                        mSimpleShaderProg.SetVec3("uniform_Color", patchColor / 2.f);
                    }
                    DrawCube(pt, scale);
                }
            }

            // draw river and control points
            mSimpleShaderProg.SetVec3("uniform_Color", { 0, 0.5, 1 });
            for (auto& pt : terrain.mRiver.mRiverCtrlPts)
            {
                DrawCube(pt, (10.f * 3) / terrain.mDivCount);
            }
            /*mSimpleShaderProg.SetVec3("uniform_Color", { 0.2, 0.7, 0.8 });
            for (int i = 0; i < river.GetMesh().size(); ++i)
            {
                float distance;
                glm::vec3 forward;
                if (i == 0)
                {
                    distance = glm::distance(river.GetMesh()[i], river.GetMesh()[i + 1]);
                    forward = glm::normalize(river.GetMesh()[i + 1] - river.GetMesh()[i]);
                }
                else
                {
                    distance = glm::distance(river.GetMesh()[i - 1], river.GetMesh()[i]);
                    forward = glm::normalize(river.GetMesh()[i] - river.GetMesh()[i - 1]);
                }
                DrawRiver(river.GetMesh()[i], { 5, 0.4, ceil(distance) }, forward, river.GetNormals()[i]);
            }*/

            mSimpleShaderProg.Unuse();

            mTriangleShaderProg.Use();
            mTriangleShaderProg.SetVec4("uniform_color", { 0.2, 0.7, 0.8, 0.f });
            DrawTriangles(terrain.mRiver.GetMesh());
            mTriangleShaderProg.SetVec4("uniform_color", { 0.f, 0.f, 0.f, 1.f });
            DrawTriangles(TriangulateMesh(patch));
            mTriangleShaderProg.Unuse();
        }
        
    }
    void Renderer::DrawRiver(glm::vec3 pos, glm::vec3 scale, glm::vec3 forward, glm::vec3 normal)
    {   
        glm::vec3 right = glm::normalize(glm::cross(normal, forward));
        glm::vec3 up = glm::normalize(glm::cross(forward, right));
        glm::mat4 rotation = glm::mat4(1.0f);
        rotation[0] = glm::vec4(right, 0.0f);
        rotation[1] = glm::vec4(up, 0.0f);
        rotation[2] = glm::vec4(forward, 0.0f);
        rotation[3] = glm::vec4(0, 0, 0, 1.0f);
        glm::mat4 model = glm::translate(glm::mat4(1.0f), pos);
        model *= rotation;
        model = glm::scale(model, scale);

        glBindVertexArray(mVAO);

        mSimpleShaderProg.SetMat4("uniform_Model", model);

        // Draw cube
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

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
    void Renderer::DrawTriangles(const std::vector<glm::vec3>& triangles)
    {
        if (triangles.empty())
            return;

        ReCreateTriangleArray(triangles);

        if (wireframe)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        glBindVertexArray(mVAOtr);

        mTriangleShaderProg.SetMat4("uniform_Model", glm::mat4(1.0));

        // Draw triangles
        glDrawArrays(GL_TRIANGLES, 0, triangles.size());

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        glBindVertexArray(0);
    }

    void Renderer::DrawLines(const std::vector<glm::vec3>& lines)
    {
        if (lines.empty())
            return;

        ReCreateLineArray(lines);

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        glBindVertexArray(mVAOln);

        // Draw triangles
        glDrawArrays(GL_LINES, 0, lines.size());

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        glBindVertexArray(0);
    }

    void Renderer::CreateTriangleArray()
    {
        glGenVertexArrays(1, &mVAOtr);
        glGenBuffers(1, &mVBOtr);

        glBindVertexArray(mVAOtr);

        // 4. Bind and upload vertex data
        glBindBuffer(GL_ARRAY_BUFFER, mVBOtr);
        glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);  // No data yet

        // 6. Define vertex attributes (position, color, texture, normal)
        glEnableVertexAttribArray(0); // Position
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

        // 7. Unbind to prevent accidental changes
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void Renderer::ReCreateTriangleArray(const std::vector<glm::vec3>& triangles)
    {
        glBindVertexArray(mVAOtr);

        glBindBuffer(GL_ARRAY_BUFFER, mVBOtr);
        glBufferData(GL_ARRAY_BUFFER, triangles.size() * sizeof(glm::vec3), triangles.data(), GL_STATIC_DRAW);

        glBindVertexArray(0);
    }

    void Renderer::ReCreateLineArray(const std::vector<glm::vec3>& lines)
    {
        glBindVertexArray(mVAOln);

        glBindBuffer(GL_ARRAY_BUFFER, mVBOln);
        glBufferData(GL_ARRAY_BUFFER, lines.size() * sizeof(glm::vec3), lines.data(), GL_STATIC_DRAW);

        glBindVertexArray(0);
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

    void Renderer::CreateLine()
    {
        glGenVertexArrays(1, &mVAOln);
        glGenBuffers(1, &mVBOln);

        glBindVertexArray(mVAOln);

        // 4. Bind and upload vertex data
        glBindBuffer(GL_ARRAY_BUFFER, mVBOln);
        glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);  // No data yet

        // 6. Define vertex attributes (position, color, texture, normal)
        glEnableVertexAttribArray(0); // Position
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

        // 7. Unbind to prevent accidental changes
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    std::vector<glm::vec3> Renderer::TriangulateMesh(const Patch& patch)
    {
        std::vector<glm::vec3> triangulatedMesh;

        int divisions = patch.GetStepCount();

        for (int x = 0; x < divisions; x++)
        {
            for (int y = 0; y < divisions; y++)
            {
                glm::vec3 botLeft = patch.mesh[y * (divisions + 1) + (x)];
                glm::vec3 botRigth = patch.mesh[y * (divisions + 1) + (x + 1)];
                glm::vec3 topLeft = patch.mesh[(y + 1)* (divisions + 1) + (x)];
                glm::vec3 topRigth = patch.mesh[(y + 1)* (divisions + 1) + (x + 1)];

                //triangulate
                triangulatedMesh.push_back(topRigth);
                triangulatedMesh.push_back(botRigth);
                triangulatedMesh.push_back(topLeft);

                triangulatedMesh.push_back(botLeft);
                triangulatedMesh.push_back(topLeft);
                triangulatedMesh.push_back(botRigth);
            }

        }




        return triangulatedMesh;
    }
    std::vector<glm::vec3> Renderer::LineMesh(const std::vector<glm::vec3>& line)
    {
        std::vector<glm::vec3> res;
        for (int i = 0; i < line.size() - 1; i++)
        {
            res.push_back(line[i]);
            res.push_back(line[i + 1]);
        }
        return res;
    }
}