#pragma once

#include <string>

namespace Engine
{
    // A rectangle in pixel coordinates within a texture.
    // Used to select a sub-region for sprite sheet rendering.
    struct TextureRect
    {
        float x = 0.0f;       // Left edge in pixels
        float y = 0.0f;       // Top edge in pixels
        float width = 0.0f;
        float height = 0.0f;
    };

    class Texture2D
    {
    public:
        // Loads an image from disk and uploads it to the GPU.
        explicit Texture2D(const std::string& filePath);
        ~Texture2D();

        // Non-copyable — the GPU texture handle must have a single owner.
        Texture2D(const Texture2D&) = delete;
        Texture2D& operator=(const Texture2D&) = delete;

        // Movable — ownership can transfer.
        Texture2D(Texture2D&& other) noexcept;
        Texture2D& operator=(Texture2D&& other) noexcept;

        // Bind/unbind the texture for rendering.
        void Bind() const;
        void Unbind() const;

        int GetWidth() const { return m_width; }
        int GetHeight() const { return m_height; }
        unsigned int GetID() const { return m_textureID; }

    private:
        unsigned int m_textureID = 0;
        int m_width = 0;
        int m_height = 0;
    };
}
