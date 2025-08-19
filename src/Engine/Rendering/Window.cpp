#include "Window.h"
#include <iostream>

namespace Engine
{
    Window::Window(const std::string &title, int width, int height)
        : m_Window(nullptr), m_Title(title), m_Width(width), m_Height(height)
    {
    }

    Window::~Window()
    {
        Shutdown();
    }

    bool Window::Initialize()
    {
        // Initialize GLFW
        if (!glfwInit())
        {
            std::cerr << "Failed to initialize GLFW" << std::endl;
            return false;
        }

        // Create window
        m_Window = glfwCreateWindow(m_Width, m_Height, m_Title.c_str(), nullptr, nullptr);
        if (!m_Window)
        {
            std::cerr << "Failed to create window" << std::endl;
            glfwTerminate();
            return false;
        }

        // Make the OpenGL context current
        glfwMakeContextCurrent(m_Window);

        // Disable V-Sync for unlimited framerate
        glfwSwapInterval(0);

        std::cout << "Window initialized successfully" << std::endl;
        return true;
    }

    void Window::PollEvents()
    {
        glfwPollEvents();
    }

    void Window::SwapBuffers()
    {
        glfwSwapBuffers(m_Window);
    }

    bool Window::ShouldClose() const
    {
        return glfwWindowShouldClose(m_Window);
    }

    void Window::Shutdown()
    {
        if (m_Window)
        {
            glfwDestroyWindow(m_Window);
            m_Window = nullptr;
        }
        glfwTerminate();
        std::cout << "Window shut down" << std::endl;
    }
}
