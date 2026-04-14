#pragma once

namespace Engine
{
    class Color
    {
    public:
        float r, g, b, a;

        // --- Constructors ---
        constexpr Color(float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 1.0f)
            : r(r), g(g), b(b), a(a) {}

        // --- Operators ---
        Color operator-() const;
        Color operator+(const Color &other) const;
        Color operator-(const Color &other) const;
        Color operator*(float scalar) const;
        Color operator/(float scalar) const;

        // --- Mathematical functions ---
        Color Lerp(const Color &other, float t) const;
        Color Multiply(const Color &other) const;

        // --- Constants ---
        static const Color Red;
        static const Color Green;
        static const Color Blue;
        static const Color White;
        static const Color Black;
    };

    inline constexpr Color Color::Red{1.0f, 0.0f, 0.0f, 1.0f};
    inline constexpr Color Color::Green{0.0f, 1.0f, 0.0f, 1.0f};
    inline constexpr Color Color::Blue{0.0f, 0.0f, 1.0f, 1.0f};
    inline constexpr Color Color::White{1.0f, 1.0f, 1.0f, 1.0f};
    inline constexpr Color Color::Black{0.0f, 0.0f, 0.0f, 1.0f};
}