#pragma once

#include "camera.hpp"
#include "window.hpp"


namespace mat300_terrain {

    class CameraControler
    {
    public:

        void HandleInput(float dt, GLFWwindow* window, Camera& cam, void (*SelectPatch)(float, float));
    };

}