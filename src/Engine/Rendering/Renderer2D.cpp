#include "Renderer2D.h"
#include <iostream>

namespace Engine
{
    Camera2D Renderer2D::s_Camera(10.0f);

    void Renderer2D::Initialize(GLFWwindow *window, int width, int height)
    {
        InitializeViewPort(window, width, height);

        s_Camera.SetWindowSize(width, height);

        // Enable alpha blending for transparency
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Set the framebuffer size callback
        glfwSetFramebufferSizeCallback(window, InitializeViewPort);

        std::cout << "Renderer2D initialized with camera-based world coordinate system" << std::endl;
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

    void Renderer2D::DrawTile(const Vec2 &position, const Vec2 &size, const Color &color)
    {
        // Set the color for this tile
        glColor4f(color.r, color.g, color.b, color.a);

        // Calculate the corners of the rectangle in world coordinates
        float halfWidth = size.x * 0.5f;
        float halfHeight = size.y * 0.5f;

        Vec2 bottomLeft = Vec2(position.x - halfWidth, position.y - halfHeight);
        Vec2 topRight = Vec2(position.x + halfWidth, position.y + halfHeight);

        // Convert corners to OpenGL coordinates
        Vec2 glBottomLeft = s_Camera.WorldToOpenGL(bottomLeft);
        Vec2 glTopRight = s_Camera.WorldToOpenGL(topRight);

        // Draw a filled rectangle using OpenGL immediate mode
        glBegin(GL_QUADS);
        glVertex2f(glBottomLeft.x, glBottomLeft.y); // Bottom-left
        glVertex2f(glTopRight.x, glBottomLeft.y);   // Bottom-right
        glVertex2f(glTopRight.x, glTopRight.y);     // Top-right
        glVertex2f(glBottomLeft.x, glTopRight.y);   // Top-left
        glEnd();
    }

    void Renderer2D::InitializeViewPort(GLFWwindow *window, int width, int height)
    {
        glViewport(0, 0, width, height);

        // Update camera when viewport changes
        s_Camera.SetWindowSize(width, height);
    }
}