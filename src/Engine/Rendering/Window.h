#pragma once

#include <GLFW/glfw3.h>
#include <string>

namespace Engine
{
    class Window
    {
    public:
        // --- Constructors & Destructors ---
        Window(const std::string &title = "Game Engine", int width = 800, int height = 600);
        ~Window();

        // --- Lifecycle ---
        bool Initialize();
        void PollEvents();
        void SwapBuffers();
        void Shutdown();

        // --- Accessors ---
        bool ShouldClose() const;
        GLFWwindow *GetNativeWindow() const;

    private:
        // --- Fields ---
        GLFWwindow *m_window;
        std::string m_title;
        int m_width;
        int m_height;
    };
}