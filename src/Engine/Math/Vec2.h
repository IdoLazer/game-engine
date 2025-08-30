#pragma once

namespace Engine
{
    class Vec2
    {
    public:
        float x, y;

        // Constructors
        Vec2(float x = 0.0f, float y = 0.0f);

        // Arithmetic operators
        Vec2 operator-() const;
        Vec2 operator+(const Vec2 &other) const;
        Vec2 operator-(const Vec2 &other) const;
        Vec2 operator*(float scalar) const;
        Vec2 operator/(float scalar) const;
        Vec2 &operator+=(const Vec2 &other);
        Vec2 &operator-=(const Vec2 &other);
        Vec2 &operator*=(float scalar);
        Vec2 &operator/=(float scalar);
        bool operator==(const Vec2 &other) const;
        bool operator!=(const Vec2 &other) const;

        // Friend function for scalar * Vec2
        friend Vec2 operator*(float scalar, const Vec2 &vec);
        friend Vec2 operator/(float scalar, const Vec2 &vec);

        // Mathematical functions
        float Length() const;
        Vec2 Normalized() const;
        float Dot(const Vec2 &other) const;

        // constants
        static const Vec2 Zero;
        static const Vec2 One;
    };
}