#pragma once

#include "camera.hpp"
#include "window.hpp"
#pragma once

namespace mat300_terrain {
    class CameraControler
    {
    public:
        CameraControler(std::function<void(float, float)> patch, std::function<void(float, float)> river) : SelectPatch(patch), SelectRiver(river) {}
        void HandleInput(float dt, GLFWwindow* window, Camera& cam);

    private:
        std::function<void(float, float)> SelectPatch;
        std::function<void(float, float)> SelectRiver;
        bool selected = false;
    };

}