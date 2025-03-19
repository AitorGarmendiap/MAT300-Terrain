#pragma once

#include "camera.hpp"
#include "window.hpp"


namespace mat300_terrain {

    class CameraControler
    {
    public:

        void HandleInput(GLFWwindow* window, float dt, Camera& cam);

    };

}