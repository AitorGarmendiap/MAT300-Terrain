#include "app.hpp"
#include "camera.hpp"
#include "terrain.hpp"

// std
#include <chrono>
#include <stdexcept>

namespace mat300_terrain {

    void App::Run()
    {

        mTerrain.Create(mScene.divCount, mScene.heightMapName.c_str());

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

            mWindow.Update();
        }

    }
}