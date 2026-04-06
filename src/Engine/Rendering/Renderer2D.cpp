#include "Renderer2D.h"
#include <iostream>
#include <cmath>

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
        glfwSetFramebufferSizeCallback(glfwGetCurrentContext(), nullptr);
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
        Vec2 halfSize = size * 0.5f;

        Vec2 bottomLeft = position - halfSize;
        Vec2 topRight = position + halfSize;

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

    void Renderer2D::DrawCircle(const Vec2 &center, float radius, const Color &color)
    {
        const int segments = 32; // Number of segments to approximate the circle

        // Set the color for this circle
        glColor4f(color.r, color.g, color.b, color.a);

        // Convert center to OpenGL coordinates
        Vec2 glCenter = s_Camera.WorldToOpenGL(center);
        float glRadiusX = radius / (s_Camera.GetWorldWidth() * 0.5f);
        float glRadiusY = radius / (s_Camera.GetWorldHeight() * 0.5f);

        // Draw a filled circle using triangle fan
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(glCenter.x, glCenter.y); // Center of the circle
        for (int i = 0; i <= segments; ++i)
        {
            float angle = i * 2.0f * 3.1415926f / segments;
            float x = glCenter.x + cos(angle) * glRadiusX;
            float y = glCenter.y + sin(angle) * glRadiusY;
            glVertex2f(x, y);
        }
        glEnd();
    }

    void Renderer2D::DrawTriangle(const Vec2 &point1, const Vec2 &point2, const Vec2 &point3, const Color &color)
    {
        // Set the color for this triangle
        glColor4f(color.r, color.g, color.b, color.a);

        // Convert points to OpenGL coordinates
        Vec2 glPoint1 = s_Camera.WorldToOpenGL(point1);
        Vec2 glPoint2 = s_Camera.WorldToOpenGL(point2);
        Vec2 glPoint3 = s_Camera.WorldToOpenGL(point3);

        // Draw a filled triangle using OpenGL immediate mode
        glBegin(GL_TRIANGLES);
        glVertex2f(glPoint1.x, glPoint1.y);
        glVertex2f(glPoint2.x, glPoint2.y);
        glVertex2f(glPoint3.x, glPoint3.y);
        glEnd();
    }

    void Renderer2D::DrawRectOutline(const Vec2 &position, const Vec2 &size, const Color &color, float thickness)
    {
        // Set the color for this outline
        glColor4f(color.r, color.g, color.b, color.a);

        // Calculate the corners of the rectangle in world coordinates
        Vec2 halfSize = size * 0.5f;

        Vec2 bottomLeft = position - halfSize;
        Vec2 topRight = position + halfSize;
        Vec2 topLeft = Vec2(bottomLeft.x, topRight.y);
        Vec2 bottomRight = Vec2(topRight.x, bottomLeft.y);

        // Convert corners to OpenGL coordinates
        Vec2 glBottomLeft = s_Camera.WorldToOpenGL(bottomLeft);
        Vec2 glTopRight = s_Camera.WorldToOpenGL(topRight);
        Vec2 glTopLeft = s_Camera.WorldToOpenGL(topLeft);
        Vec2 glBottomRight = s_Camera.WorldToOpenGL(bottomRight);

        float thicknessX = thickness * halfSize.x;
        float thicknessY = thickness * halfSize.y;
        float glThicknessX = thicknessX / (s_Camera.GetWorldWidth() * 0.5f);
        float glThicknessY = thicknessY / (s_Camera.GetWorldHeight() * 0.5f);

        // Draw the outline as four rectangles (quads)
        glBegin(GL_QUADS);
        // Bottom edge
        glVertex2f(glBottomLeft.x, glBottomLeft.y);
        glVertex2f(glBottomRight.x, glBottomRight.y);
        glVertex2f(glBottomRight.x, glBottomRight.y + glThicknessY);
        glVertex2f(glBottomLeft.x, glBottomLeft.y + glThicknessY);

        // Right edge
        glVertex2f(glBottomRight.x - glThicknessX, glBottomRight.y + glThicknessY);
        glVertex2f(glTopRight.x - glThicknessX, glTopRight.y);
        glVertex2f(glTopRight.x, glTopRight.y);
        glVertex2f(glBottomRight.x, glBottomRight.y + glThicknessY);

        // Top edge
        glVertex2f(glTopLeft.x, glTopLeft.y - glThicknessY);
        glVertex2f(glTopRight.x - glThicknessX, glTopRight.y - glThicknessY);
        glVertex2f(glTopRight.x - glThicknessX, glTopRight.y);
        glVertex2f(glTopLeft.x, glTopLeft.y);

        // Left edge
        glVertex2f(glBottomLeft.x, glBottomLeft.y + glThicknessY);
        glVertex2f(glTopLeft.x, glTopLeft.y - glThicknessY);
        glVertex2f(glTopLeft.x + glThicknessX, glTopLeft.y - glThicknessY);
        glVertex2f(glBottomLeft.x + glThicknessX, glBottomLeft.y + glThicknessY);

        glEnd();
    }

    void Renderer2D::InitializeViewPort(GLFWwindow *window, int width, int height)
    {
        glViewport(0, 0, width, height);

        // Update camera when viewport changes
        s_Camera.SetWindowSize(width, height);
    }
}