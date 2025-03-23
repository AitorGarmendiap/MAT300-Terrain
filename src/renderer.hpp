#pragma once
#include "camera.hpp"

namespace mat300_terrain {
    class Renderer
    {
    public:
        Renderer() {}
        ~Renderer() {}

        Renderer(const Renderer&) = delete;
        Renderer& operator=(const Renderer&) = delete;

        void Update(float dt);

    private:


        Camera mCamera;
    };
}