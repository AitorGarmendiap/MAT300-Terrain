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

    App::App() : mCamController([this](float x, float y) { this->SelectPatch(x, y); }, [this](float x, float y) { this->SelectRiver(x, y); })
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

            mTerrain.Update(mCamera.mTransform.translation, mCamera.mFar);

            mRenderer.Update(mCamera, mTerrain.GetPatches(), mTerrain.mRiver, mTerrain.mDivCount);

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

        // patch guizmo
        if (!mTerrain.GetPatches().empty() && mRenderer.SelectedPatch >= 0 && mRenderer.SelectedPoint >= 0)
        {
            int i = mRenderer.SelectedPoint / 4, j = mRenderer.SelectedPoint % 4;
            glm::vec3 pointPrevPos = mTerrain.GetPatches()[mRenderer.SelectedPatch].controlPoints[i][j];

            if (Guizmo(&mTerrain.GetPatches()[mRenderer.SelectedPatch].controlPoints[i][j], mCamera.GetView(), mCamera.GetProjection(), true))
                mTerrain.Recalculate(mRenderer.SelectedPatch, mRenderer.SelectedPoint, pointPrevPos);
        }
        // river guizmo
        if (mTerrain.mRiver.selectedCtrlPt >= 0)
        {
            glm::vec3 pointPrevPos = mTerrain.mRiver.mRiverCtrlPts[mTerrain.mRiver.selectedCtrlPt];

            if (Guizmo(&mTerrain.mRiver.mRiverCtrlPts[mTerrain.mRiver.selectedCtrlPt], mCamera.GetView(), mCamera.GetProjection(), false))
                mTerrain.mRiver.Recalculate(mTerrain.GetPatches(), mTerrain.mDivCount);
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
                            // remove river
                            mTerrain.mRiver.Remove();
                            // Load new scene
                            currentScene = scene;
                            mScene.LoadDataFromFile(scene.c_str());
                            mTerrain.LoadHeightMap(mScene.divCount, mScene.heightMapName.c_str());
                            mCamera.mTransform.translation = mScene.camPos;
                            mCamera.mForward = glm::vec3(0.0f, 0.0f, 1.0f);
                        }
                    }
                    ImGui::EndCombo();
                }
                ImGui::SliderInt("Patches count", &mTerrain.mDivCount, 1, 100);
                ImGui::Checkbox("Sharp edges", &mTerrain.sharpEdges);
                ImGui::Checkbox("Detailed depending on camera pos", &mTerrain.detailedPatch);
                ImGui::TreePop();
            }
            if (ImGui::TreeNodeEx("Render", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::Checkbox("Wireframe mode", &mRenderer.wireframe);
                ImGui::Checkbox("Draw control points", &mRenderer.drawControlPoints);
                ImGui::TreePop();
            }
            if (ImGui::TreeNodeEx("Input", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::Text("Move Camera: WASD QE");
                ImGui::Text("Rotate Camera: RIGHT CLICK");
                ImGui::Text("Select Patch: CTRL + LEFT CLICK");
                ImGui::Text("Select River: SHIFT + LEFT CLICK");
                ImGui::TreePop();
            }
        }
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    bool App::Guizmo(glm::vec3* position, const glm::mat4& v, const glm::mat4& p, bool onlyY)
    {
        ImGuizmo::OPERATION op = ImGuizmo::TRANSLATE;
        if (onlyY) op = ImGuizmo::TRANSLATE_Y;
        auto m2w = glm::translate(glm::mat4(1.f), *position);
        ImGuizmo::SetRect(0, 0, WIDTH, HEIGHT);
        if (ImGuizmo::Manipulate(&v[0][0], &p[0][0], op, ImGuizmo::WORLD, &m2w[0][0], NULL, NULL)) {
            float matrixTranslation[3], matrixRotation[3], matrixScale[3];
            ImGuizmo::DecomposeMatrixToComponents(&m2w[0][0], matrixTranslation, matrixRotation, matrixScale);
            *position = { matrixTranslation[0], matrixTranslation[1], matrixTranslation[2] };
            return true;
        }
        return false;
    }

    void App::SelectRiver(float mouseX, float mouseY)
    {
        // convert screen coordinate to world
        glm::vec3 worldPos = glm::unProject(glm::vec3(mouseX, HEIGHT - mouseY, 1.0), mCamera.GetView(), mCamera.GetProjection(), glm::vec4(0, 0, WIDTH, HEIGHT));
        glm::vec3 rayMouse = glm::normalize(worldPos - mCamera.GetPosition());

        // select control point
        if (mTerrain.mRiver.start && mTerrain.mRiver.end)
        {
            for (int i = 0; i < mTerrain.mRiver.mRiverCtrlPts.size(); ++i)
            {
                glm::vec3 dist = mCamera.GetPosition() - mTerrain.mRiver.mRiverCtrlPts[i];
                float proj = glm::dot(rayMouse, dist);
                float dist2 = glm::dot(dist, dist) - 5.f * 5.f;
                float discriminant = proj * proj - dist2;
                // check intersection
                if (discriminant >= 0)
                {
                    mRenderer.SelectedPoint = -1;
                    mTerrain.mRiver.selectedCtrlPt = i;
                    return;
                }
            }
            mTerrain.mRiver.selectedCtrlPt = -1;
        }
        else
        {
            // select patch
            const std::vector<Patch>& patches = mTerrain.GetPatches();
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

            if (closestPatch < 0) return;
            glm::vec3 pos = (patches[closestPatch].controlPoints[0][0] + patches[closestPatch].controlPoints[3][3]) / 2.f;
            if (!mTerrain.mRiver.start)
            {
                mTerrain.mRiver.start = true;
                mTerrain.mRiver.mRiverCtrlPts.push_back({ pos });
            }
            else // create river
            {
                mTerrain.mRiver.end = true;
                glm::vec3 start = mTerrain.mRiver.mRiverCtrlPts.front();
                glm::vec3 dir = pos - start;
                mTerrain.mRiver.mRiverCtrlPts.push_back({ start + dir * 0.3f });
                mTerrain.mRiver.mRiverCtrlPts.push_back({ start + dir * 0.7f });
                mTerrain.mRiver.mRiverCtrlPts.push_back({ pos });
                mTerrain.mRiver.Create(mTerrain.mWidth, mTerrain.mHeight);
                mTerrain.mRiver.UpdateMesh(patches, mTerrain.mDivCount);
            }
        }
    }

    void App::SelectPatch(float mouseX, float mouseY)
    {
        // convert screen coordinate to world
        glm::vec3 worldPos = glm::unProject(glm::vec3(mouseX, HEIGHT - mouseY, 1.0), mCamera.GetView(), mCamera.GetProjection(), glm::vec4(0, 0, WIDTH, HEIGHT));
        glm::vec3 rayMouse = glm::normalize(worldPos - mCamera.GetPosition());
        
        const std::vector<Patch>& patches = mTerrain.GetPatches();
        // if patch selected now select point
        if (mRenderer.SelectedPatch >= 0)
        {
            int closestPoint = PointIntersection(mCamera.GetPosition(), rayMouse, patches[mRenderer.SelectedPatch]);
            if (closestPoint >= 0)
            {
                mTerrain.mRiver.selectedCtrlPt = -1;
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