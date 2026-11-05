#include "Renderer2D.h"
#include <iostream>
#include <cmath>

namespace Engine
{
    // --- Fields ---

    Camera2D Renderer2D::s_Camera(10.0f);

    // --- Lifecycle ---

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

    // --- Frame ---

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

    // --- Drawing ---

    void Renderer2D::DrawTile(const Vec2 &worldPosition, const Vec2 &worldSize, const Color &color)
    {
        // Set the color for this tile
        glColor4f(color.r, color.g, color.b, color.a);

        // Calculate the corners of the rectangle in world coordinates
        Vec2 worldHalfSize = worldSize * 0.5f;

        Vec2 worldBottomLeft = worldPosition - worldHalfSize;
        Vec2 worldTopRight = worldPosition + worldHalfSize;

        // Convert corners to OpenGL coordinates
        Vec2 glBottomLeft = s_Camera.WorldToOpenGL(worldBottomLeft);
        Vec2 glTopRight = s_Camera.WorldToOpenGL(worldTopRight);

        // Draw a filled rectangle using OpenGL immediate mode
        glBegin(GL_QUADS);
        glVertex2f(glBottomLeft.x, glBottomLeft.y); // Bottom-left
        glVertex2f(glTopRight.x, glBottomLeft.y);   // Bottom-right
        glVertex2f(glTopRight.x, glTopRight.y);     // Top-right
        glVertex2f(glBottomLeft.x, glTopRight.y);   // Top-left
        glEnd();
    }

    void Renderer2D::DrawCircle(const Vec2 &worldCenter, float radius, const Color &color)
    {
        const int segments = 32; // Number of segments to approximate the circle

        // Set the color for this circle
        glColor4f(color.r, color.g, color.b, color.a);

        // Convert worldCenter to OpenGL coordinates
        Vec2 glCenter = s_Camera.WorldToOpenGL(worldCenter);
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

    void Renderer2D::DrawTriangle(const Vec2 &worldPoint1, const Vec2 &worldPoint2, const Vec2 &worldPoint3, const Color &color)
    {
        // Set the color for this triangle
        glColor4f(color.r, color.g, color.b, color.a);

        // Convert points to OpenGL coordinates
        Vec2 glPoint1 = s_Camera.WorldToOpenGL(worldPoint1);
        Vec2 glPoint2 = s_Camera.WorldToOpenGL(worldPoint2);
        Vec2 glPoint3 = s_Camera.WorldToOpenGL(worldPoint3);

        // Draw a filled triangle using OpenGL immediate mode
        glBegin(GL_TRIANGLES);
        glVertex2f(glPoint1.x, glPoint1.y);
        glVertex2f(glPoint2.x, glPoint2.y);
        glVertex2f(glPoint3.x, glPoint3.y);
        glEnd();
    }

    void Renderer2D::DrawLine(const Vec2 &worldStart, const Vec2 &worldEnd, const Color &color, float thickness, LineStyle style)
    {
        // Set the color for this line
        glColor4f(color.r, color.g, color.b, color.a);

        // Calculate the direction vector of the line
        Vec2 direction = worldEnd - worldStart;
        float length = direction.Length();

        if (length == 0.0f)
            return; // Avoid division by zero

        // Normalize the direction vector
        Vec2 normalizedDir = direction / length;

        // Calculate the perpendicular vector for thickness
        Vec2 perp(-normalizedDir.y, normalizedDir.x);

        // Scale the perpendicular vector by half the thickness
        Vec2 offset = perp * (thickness * 0.5f);

        switch (style)
        {
        case Solid:
        {
            // Draw a solid line as a thick rectangle (quad)
            // Calculate the four corners of the thick line rectangle
            Vec2 worldV1 = worldStart + offset;
            Vec2 worldV2 = worldStart - offset;
            Vec2 worldV3 = worldEnd - offset;
            Vec2 worldV4 = worldEnd + offset;

            // Convert corners to OpenGL coordinates
            Vec2 glV1 = s_Camera.WorldToOpenGL(worldV1);
            Vec2 glV2 = s_Camera.WorldToOpenGL(worldV2);
            Vec2 glV3 = s_Camera.WorldToOpenGL(worldV3);
            Vec2 glV4 = s_Camera.WorldToOpenGL(worldV4);

            // Draw the thick line as a rectangle (quad)
            glBegin(GL_QUADS);
            glVertex2f(glV1.x, glV1.y);
            glVertex2f(glV2.x, glV2.y);
            glVertex2f(glV3.x, glV3.y);
            glVertex2f(glV4.x, glV4.y);
            glEnd();
            break;
        }
        case Dashed:
        {
            // Draw a dashed line by breaking it into segments
            const float dashLength = 0.1f; // Length of each dash
            const float gapLength = 0.1f;  // Length of the gap between dashes
            float totalLength = 0.0f;

            while (totalLength < length)
            {
                float segmentLength = std::min(dashLength, length - totalLength);
                Vec2 worldSegmentStart = worldStart + normalizedDir * totalLength;
                Vec2 worldSegmentEnd = worldSegmentStart + normalizedDir * segmentLength;

                // Draw the dash segment as a solid line
                DrawLine(worldSegmentStart, worldSegmentEnd, color, thickness, Solid);

                totalLength += segmentLength + gapLength;
            }
            break;
        }
        case Dotted:
        {
            // Draw a dotted line by placing small circles along the line
            const float dotSpacing = 0.3f; // Spacing between dots
            float currentLength = 0.0f;

            while (currentLength < length)
            {
                Vec2 worldDotCenter = worldStart + normalizedDir * currentLength;
                DrawCircle(worldDotCenter, thickness * 0.5f, color);

                currentLength += dotSpacing;
            }
            break;
        }
        }
    }

    void Renderer2D::DrawRectOutline(const Vec2 &worldPosition, const Vec2 &worldSize, const Color &color, float thickness)
    {
        // Set the color for this outline
        glColor4f(color.r, color.g, color.b, color.a);

        // Calculate the corners of the rectangle in world coordinates
        Vec2 worldHalfSize = worldSize * 0.5f;

        Vec2 worldBottomLeft = worldPosition - worldHalfSize;
        Vec2 worldTopRight = worldPosition + worldHalfSize;
        Vec2 worldTopLeft = Vec2(worldBottomLeft.x, worldTopRight.y);
        Vec2 worldBottomRight = Vec2(worldTopRight.x, worldBottomLeft.y);

        // Convert corners to OpenGL coordinates
        Vec2 glBottomLeft = s_Camera.WorldToOpenGL(worldBottomLeft);
        Vec2 glTopRight = s_Camera.WorldToOpenGL(worldTopRight);
        Vec2 glTopLeft = s_Camera.WorldToOpenGL(worldTopLeft);
        Vec2 glBottomRight = s_Camera.WorldToOpenGL(worldBottomRight);

        float thicknessX = thickness * worldHalfSize.x;
        float thicknessY = thickness * worldHalfSize.y;
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

    // --- Accessors ---

    Camera2D &Renderer2D::GetCamera() { return s_Camera; }

    // --- Textured Drawing ---

    void Renderer2D::DrawSprite(const Vec2 &worldPosition, const Vec2 &worldSize, const Texture2D &texture, const Color &tint)
    {
        // Enable texturing, draw a full-texture quad, then disable.
        // We enable/disable per call so untextured drawing (DrawTile, etc.)
        // continues to work without any changes.
        glEnable(GL_TEXTURE_2D);
        texture.Bind();

        // The tint color multiplies with the texture color per-pixel.
        // White (1,1,1,1) = draw as-is. Any other color tints the texture.
        glColor4f(tint.r, tint.g, tint.b, tint.a);

        Vec2 worldHalfSize = worldSize * 0.5f;
        Vec2 worldBottomLeft = worldPosition - worldHalfSize;
        Vec2 worldTopRight = worldPosition + worldHalfSize;

        Vec2 glBottomLeft = s_Camera.WorldToOpenGL(worldBottomLeft);
        Vec2 glTopRight = s_Camera.WorldToOpenGL(worldTopRight);

        // UV coordinates: (0,0) = bottom-left, (1,1) = top-right of the texture.
        // stb_image flipped the image on load, so this matches screen orientation.
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(glBottomLeft.x, glBottomLeft.y);
        glTexCoord2f(1.0f, 0.0f); glVertex2f(glTopRight.x, glBottomLeft.y);
        glTexCoord2f(1.0f, 1.0f); glVertex2f(glTopRight.x, glTopRight.y);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(glBottomLeft.x, glTopRight.y);
        glEnd();

        texture.Unbind();
        glDisable(GL_TEXTURE_2D);
    }

    void Renderer2D::DrawSprite(const Vec2 &worldPosition, const Vec2 &worldSize, const Texture2D &texture, const TextureRect &srcRect, const Color &tint)
    {
        glEnable(GL_TEXTURE_2D);
        texture.Bind();
        glColor4f(tint.r, tint.g, tint.b, tint.a);

        Vec2 worldHalfSize = worldSize * 0.5f;
        Vec2 worldBottomLeft = worldPosition - worldHalfSize;
        Vec2 worldTopRight = worldPosition + worldHalfSize;

        Vec2 glBottomLeft = s_Camera.WorldToOpenGL(worldBottomLeft);
        Vec2 glTopRight = s_Camera.WorldToOpenGL(worldTopRight);

        // Convert pixel coordinates to normalized UV coordinates [0, 1].
        // srcRect is in pixel space: (x, y) = top-left corner, (width, height) = size.
        float texW = static_cast<float>(texture.GetWidth());
        float texH = static_cast<float>(texture.GetHeight());

        float u0 = srcRect.x / texW;                       // left
        float u1 = (srcRect.x + srcRect.width) / texW;     // right
        // Note the flip: srcRect.y is from the top of the image, but UV (0,0) is
        // at the bottom (because we flipped on load). So the bottom UV = 1 - (y + height)/H.
        float v0 = 1.0f - (srcRect.y + srcRect.height) / texH;  // bottom
        float v1 = 1.0f - srcRect.y / texH;                      // top

        glBegin(GL_QUADS);
        glTexCoord2f(u0, v0); glVertex2f(glBottomLeft.x, glBottomLeft.y);
        glTexCoord2f(u1, v0); glVertex2f(glTopRight.x, glBottomLeft.y);
        glTexCoord2f(u1, v1); glVertex2f(glTopRight.x, glTopRight.y);
        glTexCoord2f(u0, v1); glVertex2f(glBottomLeft.x, glTopRight.y);
        glEnd();

        texture.Unbind();
        glDisable(GL_TEXTURE_2D);
    }

    // --- Text ---

    void Renderer2D::DrawText(const Vec2 &worldPosition, const std::string &text, const BitmapFont &font, float charHeight, const Color &color)
    {
        const Texture2D* atlas = font.GetTexture();
        if (!atlas)
            return;

        float charWidth = charHeight * font.GetAspectRatio();

        // worldPosition is top-left anchored. DrawSprite uses center, so offset by half.
        float cursorX = worldPosition.x + charWidth * 0.5f;
        float cursorY = worldPosition.y - charHeight * 0.5f;

        Vec2 charSize(charWidth, charHeight);

        for (char c : text)
        {
            TextureRect srcRect = font.GetGlyphRect(c);

            if (srcRect.width > 0.0f && srcRect.height > 0.0f)
                DrawSprite(Vec2(cursorX, cursorY), charSize, *atlas, srcRect, color);

            cursorX += charWidth;
        }
    }

    // --- Internal ---

    void Renderer2D::InitializeViewPort(GLFWwindow *window, int width, int height)
    {
        glViewport(0, 0, width, height);

        // Update camera when viewport changes
        s_Camera.SetWindowSize(width, height);
    }
}