#pragma once

#include <GLFW/glfw3.h>
#include <string>

namespace Engine
{
    class Window
    {
    public:
        Window(const std::string &title = "Game Engine", int width = 800, int height = 600);
        ~Window();

        bool Initialize();
        void PollEvents();
        void SwapBuffers();
        void Shutdown();

        bool ShouldClose() const;
        GLFWwindow *GetNativeWindow() const { return m_Window; }

    private:
        GLFWwindow *m_Window;
        std::string m_Title;
        int m_Width;
        int m_Height;
    };
}