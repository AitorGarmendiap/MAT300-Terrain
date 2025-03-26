#include "app.hpp"
#include "camera.hpp"
#include "terrain.hpp"

// std
#include <chrono>
#include <stdexcept>

// Imgui
#include <imgui.h>              
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

namespace mat300_terrain {

    App::App() : mCamController([this](float x, float y) { this->SelectPatch(x, y); })
    {
        CreateImgui();
    }

    void App::Run()
    {
        mTerrain.Create(mScene.patchCount, mScene.heightMapName.c_str());

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

            mTerrain.Update(dt);

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

        if (ImGui::Begin("Demo options"))
        {
            ImGui::Text("FPS: %f", ImGui::GetIO().Framerate);
            ImGui::Text("dt: %f", dt);
            if (ImGui::TreeNode("Patches"))
            {
                ImGui::SliderInt("Patches size", &mScene.patchCount, 1, 20);
                ImGui::TreePop();
            }
        }
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void App::SelectPatch(float mouseX, float mouseY)
    {
        glm::vec3 worldPos = glm::unProject(glm::vec3(mouseX, mouseY, 1.0), mCamera.GetView(), mCamera.GetProjection(), glm::vec4(0, 0, WIDTH, HEIGHT));
        glm::vec3 rayMouse = glm::normalize(worldPos - mCamera.GetPosition());

        int closestPatch = -1;
        float distance = 0;
        const std::vector<Patch>& patches = mTerrain.GetPatches();
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
        if (closestPatch != SelectedPatch)
        {
            SelectedPatch = closestPatch;
            // remove guizmo from selected point
        }
        else // select point
        {

        }
    }

    bool App::PatchIntersection(glm::vec3 origin, glm::vec3 dir, Patch patch)
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
}