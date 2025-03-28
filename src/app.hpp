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

    private:

        void CreateImgui();
        void UpdateImgui(float dt);
        bool Guizmo(glm::vec3* position, const glm::mat4& v, const glm::mat4& p);
        void SelectPatch(float mouseX, float mouseY);
        bool PatchIntersection(glm::vec3 origin, glm::vec3 dir, const Patch& patch);
        int PointIntersection(glm::vec3 origin, glm::vec3 dir, const Patch& patch, float radius = 5);

        static constexpr int WIDTH = 1280;
        static constexpr int HEIGHT = 720;

        Window mWindow{ WIDTH, HEIGHT, "mat300 terrain" };

        std::string scenePath = "data/scenes/";
        std::string currentScene = scenePath + "Bilbao.txt";
        Scene mScene{ currentScene.c_str() };
        std::vector<std::string> scenes;

        Camera mCamera;
        CameraControler mCamController;

        Renderer mRenderer{};
        Terrain mTerrain;

        int SelectedPatch = 0;
        int SelectedPoint = 5;
    };
}