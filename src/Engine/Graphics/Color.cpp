#include "Color.h"

namespace Engine
{
    static const Color Red(1.0f, 0.0f, 0.0f, 1.0f);
    static const Color Green(0.0f, 1.0f, 0.0f, 1.0f);
    static const Color Blue(0.0f, 0.0f, 1.0f, 1.0f);
    static const Color White(1.0f, 1.0f, 1.0f, 1.0f);
    static const Color Black(0.0f, 0.0f, 0.0f, 1.0f);

    Color::Color(float r, float g, float b, float a)
    {
        this->r = r;
        this->g = g;
        this->b = b;
        this->a = a;
    }

    Color Color::operator-() const
    {
        return Color(-r, -g, -b, -a);
    }

    Color Color::operator+(const Color &other) const
    {
        return Color(r + other.r, g + other.g, b + other.b, a + other.a);
    }

    Color Color::operator-(const Color &other) const
    {
        return *this + (-other);
    }

    Color Color::operator*(float scalar) const
    {
        return Color(r * scalar, g * scalar, b * scalar, a * scalar);
    }

    Color Color::operator/(float scalar) const
    {
        return Color(r / scalar, g / scalar, b / scalar, a / scalar);
    }

    Color Color::Lerp(const Color &other, float t) const
    {
        return Color(
            r + (other.r - r) * t,
            g + (other.g - g) * t,
            b + (other.b - b) * t,
            a + (other.a - a) * t);
    }

    Color Color::Multiply(const Color &other) const
    {
        return Color(r * other.r, g * other.g, b * other.b, a * other.a);
    }
}