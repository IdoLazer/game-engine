#pragma once

#include <GLFW/glfw3.h>
#include <unordered_map>

namespace Engine
{
    class Keyboard
    {
    public:
        static void Initialize(GLFWwindow *window);
        static void Shutdown();
        static void Update();
        static bool IsKeyDown(int glfwKey);
        static bool IsKeyPressed(int glfwKey);
        static bool IsKeyReleased(int glfwKey);

    private:
        static GLFWwindow *m_window;
        static std::unordered_map<int, bool> m_currentKeyState;
        static std::unordered_map<int, bool> m_previousKeyState;
        static bool m_initialized;

        static void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
    };
}