#pragma once

#include "window.hpp"

namespace mat300_terrain {
    class App
    {
    public:
        App(){}
        ~App(){}

        App(const App&) = delete;
        App& operator=(const App&) = delete;
        void Run();

    private:

        static constexpr int WIDTH = 1280;
        static constexpr int HEIGHT = 720;

        Window mWindow{ WIDTH, HEIGHT, "AFL cs562" };
    };
}