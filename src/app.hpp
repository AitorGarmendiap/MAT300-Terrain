#pragma once

#include "window.hpp"
#include "scene_parser.hpp"
#include "camera.hpp"
#include "camera_controller.hpp"
#include "renderer.hpp"
#include "terrain.hpp"

namespace mat300_terrain {
    class App
    {
    public:
        App();
        ~App(){}

        App(const App&) = delete;
        App& operator=(const App&) = delete;

        void Run();
        void SelectPatch(float mouseX, float mouseY);

    private:

        void CreateImgui();
        void UpdateImgui(float dt);
        bool PatchIntersection(glm::vec3 origin, glm::vec3 dir, Patch patch);

        static constexpr int WIDTH = 1280;
        static constexpr int HEIGHT = 720;

        Window mWindow{ WIDTH, HEIGHT, "mat300 terrain" };

        Scene mScene{ "data/scenes/scene_01.txt" };

        Camera mCamera;
        CameraControler mCamController;

        Renderer mRenderer{};
        Terrain mTerrain;
    };
}