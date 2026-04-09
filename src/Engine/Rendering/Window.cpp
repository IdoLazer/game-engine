#include "Window.h"
#include <iostream>

namespace Engine
{
    Window::Window(const std::string &title, int width, int height)
        : m_window(nullptr), m_title(title), m_width(width), m_height(height)
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
        m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);
        if (!m_window)
        {
            std::cerr << "Failed to create window" << std::endl;
            glfwTerminate();
            return false;
        }

        // Make the OpenGL context current
        glfwMakeContextCurrent(m_window);

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
        glfwSwapBuffers(m_window);
    }

    bool Window::ShouldClose() const
    {
        return glfwWindowShouldClose(m_window);
    }

    void Window::Shutdown()
    {
        if (m_window)
        {
            glfwDestroyWindow(m_window);
            m_window = nullptr;
        }
        glfwTerminate();
        std::cout << "Window shut down" << std::endl;
    }
}
