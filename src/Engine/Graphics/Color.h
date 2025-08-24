#pragma once

namespace Engine
{
    class Color
    {
    public:
        float r, g, b, a;

        // Constructors
        Color(float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 1.0f);

        // Arithmetic operators
        Color operator-() const;
        Color operator+(const Color &other) const;
        Color operator-(const Color &other) const;
        Color operator*(float scalar) const;
        Color operator/(float scalar) const;

        // Color operations
        Color Lerp(const Color &other, float t) const;
        Color Multiply(const Color &other) const;

        // Common colors
        static const Color Red;
        static const Color Green;
        static const Color Blue;
        static const Color White;
        static const Color Black;
    };
}