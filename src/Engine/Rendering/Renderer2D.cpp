#include "Renderer2D.h"
#include <iostream>

namespace Engine
{
    void Renderer2D::Initialize(GLFWwindow *window, int width, int height)
    {
        InitializeViewPort(window, width, height);

        // Enable alpha blending for transparency
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Set the framebuffer size callback
        glfwSetFramebufferSizeCallback(window, InitializeViewPort);

        std::cout << "Renderer2D initialized with world coordinate system (0-100)" << std::endl;
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
        float left = position.x;
        float right = position.x + size.x;
        float bottom = position.y;
        float top = position.y + size.y;

        // Convert corners to OpenGL coordinates
        left = WorldToOpenGL(left);
        right = WorldToOpenGL(right);
        bottom = WorldToOpenGL(bottom);
        top = WorldToOpenGL(top);

        // Draw a filled rectangle using OpenGL immediate mode
        glBegin(GL_QUADS);
        glVertex2f(left, bottom);  // Bottom-left
        glVertex2f(right, bottom); // Bottom-right
        glVertex2f(right, top);    // Top-right
        glVertex2f(left, top);     // Top-left
        glEnd();
    }

    void Renderer2D::InitializeViewPort(GLFWwindow *window, int width, int height)
    {
        glViewport(0, 0, width, height);
    }

    float Renderer2D::WorldToOpenGL(float worldCoord)
    {
        // Convert from world coordinates (0-100) to OpenGL coordinates (-1 to +1)
        return (worldCoord / 100.0f) * 2.0f - 1.0f;
    }

    Vec2 Renderer2D::WorldToOpenGL(const Vec2 &worldPos)
    {
        return Vec2(WorldToOpenGL(worldPos.x), WorldToOpenGL(worldPos.y));
    }
}