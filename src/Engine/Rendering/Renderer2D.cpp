#include "Renderer2D.h"
#include <iostream>

namespace Engine
{
    void Renderer2D::Initialize(GLFWwindow *window, int width, int height)
    {
        InitializeViewPort(window, width, height);
        glfwSetFramebufferSizeCallback(window, InitializeViewPort);
        std::cout << "Renderer2D initialized" << std::endl;
    }

    void Renderer2D::Shutdown()
    {
        std::cout << "Renderer2D shutdown" << std::endl;
    }

    void Renderer2D::BeginFrame()
    {
        // Nothing needed here for now
    }

    void Renderer2D::EndFrame()
    {
        // Nothing needed here for now
    }

    void Renderer2D::Clear(const Color &color)
    {
        glClearColor(color.r, color.g, color.b, color.a);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    void Renderer2D::InitializeViewPort(GLFWwindow *window, int width, int height)
    {
        glViewport(0, 0, width, height);
    }
}