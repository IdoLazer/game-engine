// This MUST be defined in exactly one .cpp file in the project.
// It tells stb_image.h to generate the actual function implementations,
// not just declarations. Every other file that includes stb_image.h
// gets only the declarations (like a normal header).
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "Texture2D.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <utility>

// GL_CLAMP_TO_EDGE is OpenGL 1.2 but GLFW's built-in gl.h only covers 1.1.
// The constant value is stable across all platforms — safe to define directly.
#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE 0x812F
#endif

namespace Engine
{
    Texture2D::Texture2D(const std::string& filePath)
    {
        // stb_image loads with (0,0) at top-left, but OpenGL expects
        // (0,0) at bottom-left. This flips the image vertically on load
        // so UV coordinates work correctly: (0,0) = bottom-left.
        stbi_set_flip_vertically_on_load(true);

        int channels = 0;
        unsigned char* pixels = stbi_load(filePath.c_str(), &m_width, &m_height, &channels, 0);

        if (!pixels)
        {
            std::cerr << "Texture2D: Failed to load image: " << filePath << std::endl;
            return;
        }

        // Determine the OpenGL format based on the number of color channels.
        // RGB (3 channels) for images without transparency, RGBA (4) for those with.
        GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;

        // Generate and configure the OpenGL texture object.
        glGenTextures(1, &m_textureID);
        glBindTexture(GL_TEXTURE_2D, m_textureID);

        // Upload pixel data to the GPU.
        glTexImage2D(GL_TEXTURE_2D, 0, format, m_width, m_height, 0, format, GL_UNSIGNED_BYTE, pixels);

        // Filtering: how OpenGL samples the texture when it's scaled.
        // GL_NEAREST preserves sharp pixel edges (good for pixel art / chess pieces).
        // GL_LINEAR for smooth bilinear filtering if needed later.
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Wrapping: what happens when UV coords go outside [0,1].
        // GL_CLAMP_TO_EDGE prevents border artifacts on sprite edges.
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glBindTexture(GL_TEXTURE_2D, 0);

        // The CPU-side pixel data is now on the GPU — we don't need it anymore.
        stbi_image_free(pixels);

        std::cout << "Texture2D: Loaded " << filePath
                  << " (" << m_width << "x" << m_height << ", " << channels << " channels)" << std::endl;
    }

    Texture2D::~Texture2D()
    {
        if (m_textureID != 0)
        {
            glDeleteTextures(1, &m_textureID);
        }
    }

    // Move constructor — steal the GPU handle from `other`.
    Texture2D::Texture2D(Texture2D&& other) noexcept
        : m_textureID(other.m_textureID)
        , m_width(other.m_width)
        , m_height(other.m_height)
    {
        // Zero out the source so its destructor doesn't delete our texture.
        other.m_textureID = 0;
        other.m_width = 0;
        other.m_height = 0;
    }

    // Move assignment — release our current texture, then steal from `other`.
    Texture2D& Texture2D::operator=(Texture2D&& other) noexcept
    {
        if (this != &other)
        {
            if (m_textureID != 0)
            {
                glDeleteTextures(1, &m_textureID);
            }

            m_textureID = other.m_textureID;
            m_width = other.m_width;
            m_height = other.m_height;

            other.m_textureID = 0;
            other.m_width = 0;
            other.m_height = 0;
        }
        return *this;
    }

    void Texture2D::Bind() const
    {
        glBindTexture(GL_TEXTURE_2D, m_textureID);
    }

    void Texture2D::Unbind() const
    {
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}
