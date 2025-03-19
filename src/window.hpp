#pragma once

#include <glad/glad.h>  // OpenGl library
#include <GLFW/glfw3.h> // For contexts

namespace mat300_terrain {
    
    class Window
    {
    public:
        Window(const int w, const int h, const char* name);
        ~Window();

        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;

        void Clear();
        void Update();

        bool ShouldClose() const { return glfwWindowShouldClose(mWindow); }
        float GetAspectRatio() const { return static_cast<float>(mWidth) / static_cast<float>(mHeight); }
        GLFWwindow* GetGLFWWindow() const { return mWindow; }

    private:
        void Init();
        void ShutDown();

        GLFWwindow* mWindow = nullptr;

        const char* mName = "mat300 terrain";

        int mWidth = 1280;
        int mHeight = 720;
    };
}
