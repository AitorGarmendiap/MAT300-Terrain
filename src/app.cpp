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

    App::App()
    {
        CreateImgui();
    }

    void App::Run()
    {
        auto currentTime = std::chrono::high_resolution_clock::now();
        float vertFov = 60;

        float nearPlane = 0.1;
        float farPlane = 500;

        //Update
        while (mWindow.ShouldClose() == false)
        {
            auto newTime = std::chrono::high_resolution_clock::now();
            float dt = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            mWindow.Clear();

            mCamController.HandleInput(dt, mWindow.GetGLFWWindow(), mCamera, &SelectPatch);

            mCamera.SetPrespectiveProjection(vertFov, mWindow.GetAspectRatio(), nearPlane, farPlane);

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
                ImGui::SliderInt("Patches size", &mTerrain.size, 1, 20);
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

        Patches* closestPatch = nullptr;
        for (auto& patch : mTerrain.GetPatches())
        {
            if (PatchIntersection(mCamera.GetPosition(), rayMouse, patch))
            {
                // check distance
            }
        }
    }

    bool App::PatchIntersection(glm::vec3 origin, glm::vec3 dir, Patches patch)
    {
        glm::vec3 planeNormal = glm::normalize(glm::cross(patch.GetRight(), patch.GetUp()));

        // they are parallel
        float denom = glm::dot(dir, planeNormal);
        if (fabs(denom) < 1e-6) return false;

        // intersection behind
        float t = glm::dot(patch.pos - origin, planeNormal) / denom;
        if (t < 0) return false;

        // Proyect intersection
        glm::vec3 localHit = origin + t * dir - patch.pos;
        float projRight = glm::dot(localHit, glm::normalize(patch.GetRight()));
        float projUp = glm::dot(localHit, glm::normalize(patch.GetUp()));

        // Verify intersection
        return (projRight >= 0 && projRight <= glm::length(patch.GetRight()) &&
            projUp >= 0 && projUp <= glm::length(patch.GetUp()));
    }
}