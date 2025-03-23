#include "app.hpp"
#include "camera.hpp"
#include "terrain.hpp"

// std
#include <chrono>
#include <stdexcept>

namespace mat300_terrain {

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

            mCamController.HandleInput(dt, mWindow.GetGLFWWindow(), mCamera);

            mCamera.SetPrespectiveProjection(vertFov, mWindow.GetAspectRatio(), nearPlane, farPlane);

            mCamera.Update();

            mTerrain.Update(dt);

            mRenderer.Update(dt, mCamera, mTerrain.GetPatches());

            mWindow.Update();
        }

    }
}