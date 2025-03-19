#include "app.hpp"
#include "camera.hpp"
#include "terrain.hpp"

// std
#include <chrono>
#include <stdexcept>

namespace mat300_terrain {

    void App::Run()
    {
        // INIT
        Camera camera{};


        auto currentTime = std::chrono::high_resolution_clock::now();

        //Update
        while (mWindow.ShouldClose() == false)
        {
            mWindow.Clear();
            glfwPollEvents();

            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;





        }

    }
}