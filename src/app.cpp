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

        //Update
        while (mWindow.ShouldClose() == false)
        {
            auto newTime = std::chrono::high_resolution_clock::now();
            float dt = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            mWindow.Clear();

            mTerrain.Update(dt);

            mRenderer.Update(dt);

            mWindow.Update();
        }

    }
}