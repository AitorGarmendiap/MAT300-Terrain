#pragma once

#include "camera.hpp"
#include "window.hpp"
#pragma once

namespace mat300_terrain {
    class CameraControler
    {
    public:
        CameraControler(std::function<void(float, float)> func) : SelectPatch(func) {}
        void HandleInput(float dt, GLFWwindow* window, Camera& cam);

    private:
        std::function<void(float, float)> SelectPatch;
    };

}