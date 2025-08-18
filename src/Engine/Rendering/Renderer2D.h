#pragma once

#include <GLFW/glfw3.h>
#include "Window.h"

namespace Engine
{
    struct Color
    {
        float r, g, b, a;

        Color(float red = 1.0f, float green = 1.0f, float blue = 1.0f, float alpha = 1.0f)
            : r(red), g(green), b(blue), a(alpha) {}
    };

    struct Vec2
    {
        float x, y;

        Vec2(float x = 0.0f, float y = 0.0f) : x(x), y(y) {}
    };

    class Renderer2D
    {
    public:
        static void Initialize(GLFWwindow *window, int width, int height);
        static void Shutdown();

        static void BeginFrame();
        static void EndFrame();

        static void Clear(const Color &color = Color(0.0f, 0.0f, 0.0f, 1.0f));

    private:
        static void InitializeViewPort(GLFWwindow *window, int width, int height);
    };
}
