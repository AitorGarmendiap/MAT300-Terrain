#include "app.hpp"
#include "camera.hpp"
#include "terrain.hpp"

// std
#include <chrono>
#include <stdexcept>
#include <filesystem>

// Imgui
#include <imgui.h>              
#include <ImGuizmo.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

namespace mat300_terrain {

    App::App() : mCamController([this](float x, float y) { this->SelectPatch(x, y); })
    {
        CreateImgui();
        // get all scene names  
        for (const auto& entry : std::filesystem::directory_iterator(scenePath))
            scenes.push_back(entry.path().filename().string());
    }

    void App::Run()
    {
        mTerrain.LoadHeightMap(mScene.divCount, mScene.heightMapName.c_str());

        mCamera.mTransform.translation = mScene.camPos;

        auto currentTime = std::chrono::high_resolution_clock::now();

        while (mWindow.ShouldClose() == false)
        {
            auto newTime = std::chrono::high_resolution_clock::now();
            float dt = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            mWindow.Clear();

            mCamController.HandleInput(dt, mWindow.GetGLFWWindow(), mCamera);

            mCamera.SetPrespectiveProjection(mScene.FOVy, mWindow.GetAspectRatio(), mScene.nearPlane, mScene.farPlane);

            mCamera.Update();

            mTerrain.Update();

            mRenderer.Update(dt, mCamera, mTerrain.GetPatches());

            UpdateImgui(dt);

            mWindow.Update();
        }

    }

    void App::CreateImgui()
    {
        IMGUI_CHECKVERSION();
        if (!ImGui::CreateContext())
            throw std::runtime_error("Could not initialize ImGui");

        if (!ImGui_ImplGlfw_InitForOpenGL(static_cast<GLFWwindow*>(mWindow.GetGLFWWindow()), true))
            throw std::runtime_error("Could not initialize ImGui::OpenGL");

        if (!ImGui_ImplOpenGL3_Init("#version 150"))
            throw std::runtime_error("Could not initialize ImGui::OpenGL (2)");
    }

    void App::UpdateImgui(float dt)
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGuizmo::BeginFrame();

        if (!mTerrain.mPatches.empty() && mRenderer.SelectedPatch >= 0 && mRenderer.SelectedPoint >= 0)
        {
            glm::vec3 pos = mCamera.GetPosition() + glm::vec3{0, 0, -5};
            int i = mRenderer.SelectedPoint / 4, j = mRenderer.SelectedPoint % 4;
            glm::vec3 pointPrevPos = mTerrain.mPatches[mRenderer.SelectedPatch].controlPoints[i][j];

            if (Guizmo(&mTerrain.mPatches[mRenderer.SelectedPatch].controlPoints[i][j], mCamera.GetView(), mCamera.GetProjection()))
                mTerrain.Update(mRenderer.SelectedPatch, mRenderer.SelectedPoint, pointPrevPos);
        }

        ImGui::SetNextWindowPos({ 0, 0 });
        ImGui::SetNextWindowSize({ 300, HEIGHT });
        if (ImGui::Begin("Demo options", nullptr, ImGuiWindowFlags_::ImGuiWindowFlags_NoResize))
        {
            ImGui::PushItemWidth(150);
            ImGui::Text("FPS: %f", 1.f / dt);
            ImGui::Text("dt: %f", dt);
            if (ImGui::TreeNodeEx("Scene", ImGuiTreeNodeFlags_DefaultOpen))
            {
                if (ImGui::BeginCombo("Select file", currentScene.substr(scenePath.size()).c_str()))
                {
                    for (const auto& file : scenes)
                    {
                        std::string scene = scenePath + file;
                        if (ImGui::Selectable(file.c_str(), currentScene == scene))
                        {
                            // Load new scene
                            currentScene = scene;
                            mScene.LoadDataFromFile(scene.c_str());
                            mTerrain.LoadHeightMap(mScene.divCount, mScene.heightMapName.c_str());
                            mCamera.mTransform.translation = mScene.camPos;
                        }
                    }
                    ImGui::EndCombo();
                }
                ImGui::SliderInt("Patches count", &mTerrain.mDivCount, 1, 20);
                ImGui::Checkbox("Sharp edges", &mTerrain.sharpEdges);
                ImGui::TreePop();
            }
            if (ImGui::TreeNodeEx("Render", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::DragFloat3("Patch color", &mRenderer.patchColor[0], 0.01f, 0.0f, 1.0f, "%0.1f");
                ImGui::DragFloat3("Border color", &mRenderer.borderColor[0], 0.01f, 0.0f, 1.0f, "%0.1f");
                ImGui::DragFloat3("Selected color", &mRenderer.selectedColor[0], 0.01f, 0.0f, 1.0f, "%0.1f");
                ImGui::TreePop();
            }
        }
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    bool App::Guizmo(glm::vec3* position, const glm::mat4& v, const glm::mat4& p)
    {
        auto m2w = glm::translate(glm::mat4(1.f), *position);
        ImGuizmo::SetRect(0, 0, WIDTH, HEIGHT);
        if (ImGuizmo::Manipulate(&v[0][0], &p[0][0], ImGuizmo::TRANSLATE_Y, ImGuizmo::WORLD, &m2w[0][0], NULL, NULL)) {
            float matrixTranslation[3], matrixRotation[3], matrixScale[3];
            ImGuizmo::DecomposeMatrixToComponents(&m2w[0][0], matrixTranslation, matrixRotation, matrixScale);
            *position = { matrixTranslation[0], matrixTranslation[1], matrixTranslation[2] };
            return true;
        }
        return false;
    }

    void App::SelectPatch(float mouseX, float mouseY)
    {
        // convert screen coordinate to world
        glm::vec3 worldPos = glm::unProject(glm::vec3(mouseX, HEIGHT - mouseY, 1.0), mCamera.GetView(), mCamera.GetProjection(), glm::vec4(0, 0, WIDTH, HEIGHT));
        glm::vec3 rayMouse = glm::normalize(worldPos - mCamera.GetPosition());
        
        const std::vector<Patch>& patches = mTerrain.mPatches;
        // if patch selected now select point
        if (mRenderer.SelectedPatch >= 0)
        {
            int closestPoint = PointIntersection(mCamera.GetPosition(), rayMouse, patches[mRenderer.SelectedPatch]);
            if (closestPoint >= 0)
            {
                mRenderer.SelectedPoint = closestPoint;
                return;
            }
        }
        // select patch
        int closestPatch = -1;
        float distance = 0;
        for (int i = 0; i < patches.size(); ++i)
        {
            if (PatchIntersection(mCamera.GetPosition(), rayMouse, patches[i]))
            {
                glm::vec3 center = (patches[i].controlPoints[0][0] + patches[i].controlPoints[3][3]) / 2.f;
                float newDistance = glm::distance(mCamera.GetPosition(), center);
                if (closestPatch < 0 || newDistance < distance)
                {
                    closestPatch = i;
                    distance = newDistance;
                }
            }
        }
        // no patch selected
        if (closestPatch < 0)
        {
            mRenderer.SelectedPatch = -1;
            mRenderer.SelectedPoint = -1;
        }
        // change patch
        if (closestPatch != mRenderer.SelectedPatch)
        {
            mRenderer.SelectedPatch = closestPatch;
            mRenderer.SelectedPoint = -1;
        }
    }

    bool App::PatchIntersection(glm::vec3 origin, glm::vec3 dir, const Patch& patch)
    {
        // check if they are parallel
        float denom = glm::dot(patch.normal, dir);
        if (glm::abs(denom) < 1e-6) return false;

        // check if intersection is behind
        float t = glm::dot(patch.normal, patch.controlPoints[0][0] - origin) / denom;
        if (t < 0) return false;

        // Check if intersection is inside the patch
        glm::vec3 intersection = origin + t * dir;
        glm::vec3 v1 = patch.controlPoints[3][0] - patch.controlPoints[0][0]; // B - A
        glm::vec3 v2 = patch.controlPoints[3][3] - patch.controlPoints[0][0]; // D - A

        glm::vec3 intersectionLocal = intersection - patch.controlPoints[0][0];
        float u = glm::dot(intersectionLocal, v1) / glm::dot(v1, v1);
        float v = glm::dot(intersectionLocal, v2) / glm::dot(v2, v2);

        return (u >= 0 && u <= 1 && v >= 0 && v <= 1);
    }

    int App::PointIntersection(glm::vec3 origin, glm::vec3 dir, const Patch& patch, float radius)
    {
        if (patch.controlPoints.empty())
            return -1;

        int closestPoint = -1;
        float distance = 0;
        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                if (i == 0 && j == 0 || i == 0 && j == 3 || i == 3 && j == 0 || i == 3 && j == 3)
                    continue;

                glm::vec3 dist = origin - patch.controlPoints[i][j];
                float proj = glm::dot(dir, dist);
                float dist2 = glm::dot(dist, dist) - radius * radius;
                float discriminant = proj * proj - dist2;
                // check intersection
                if (discriminant >= 0)
                {
                    float newDistance = glm::distance(mCamera.GetPosition(), patch.controlPoints[i][j]);
                    if (closestPoint < 0 || newDistance < distance)
                    {
                        closestPoint = 4 * i + j;
                        distance = newDistance;
                    }
                }
            }
        }
        return closestPoint;
    }
}