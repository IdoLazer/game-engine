#include "Vec2.h"
#include <cmath>

namespace Engine
{
    static const Vec2 Zero(0.0f, 0.0f);
    static const Vec2 One(1.0f, 1.0f);

    Vec2::Vec2(float x, float y)
    {
        this->x = x;
        this->y = y;
    }

    Vec2 Vec2::operator-() const
    {
        return Vec2(-this->x, -this->y);
    }

    Vec2 Vec2::operator+(const Vec2 &other) const
    {
        return Vec2(this->x + other.x, this->y + other.y);
    }

    Vec2 Vec2::operator-(const Vec2 &other) const
    {
        return *this + (-other);
    }

    Vec2 Vec2::operator*(float scalar) const
    {
        return Vec2(this->x * scalar, this->y * scalar);
    }

    Vec2 Vec2::operator/(float scalar) const
    {
        return *this * (1.0f / scalar);
    }

    Vec2 &Vec2::operator+=(const Vec2 &other)
    {
        this->x += other.x;
        this->y += other.y;
        return *this;
    }

    Vec2 &Vec2::operator-=(const Vec2 &other)
    {
        this->x -= other.x;
        this->y -= other.y;
        return *this;
    }

    Vec2 &Vec2::operator*=(float scalar)
    {
        this->x *= scalar;
        this->y *= scalar;
        return *this;
    }

    Vec2 &Vec2::operator/=(float scalar)
    {
        this->x /= scalar;
        this->y /= scalar;
        return *this;
    }

    bool Vec2::operator==(const Vec2 &other) const
    {
        return this->x == other.x && this->y == other.y;
    }

    float Vec2::Length() const
    {
        return std::sqrt(this->x * this->x + this->y * this->y);
    }

    Vec2 Vec2::Normalized() const
    {
        float length = Length();
        if (length == 0.0f)
            return Vec2();
        return *this / length;
    }

    float Vec2::Dot(const Vec2 &other) const
    {
        return this->x * other.x + this->y * other.y;
    }
}
