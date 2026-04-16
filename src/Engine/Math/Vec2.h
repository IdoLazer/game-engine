#pragma once

namespace Engine
{
    class Vec2
    {
    public:
        float x, y;

        // --- Constructors ---
        constexpr explicit Vec2(float x = 0.0f, float y = 0.0f) : x(x), y(y) {}
        constexpr explicit Vec2(int x, int y) : x(static_cast<float>(x)), y(static_cast<float>(y)) {}

        // --- Operators ---
        Vec2 operator-() const;
        Vec2 operator+(const Vec2 &other) const;
        Vec2 operator-(const Vec2 &other) const;
        Vec2 operator*(float scalar) const;
        Vec2 operator/(float scalar) const;
        Vec2 &operator+=(const Vec2 &other);
        Vec2 &operator-=(const Vec2 &other);
        Vec2 &operator*=(float scalar);
        Vec2 &operator/=(float scalar);

        // --- Comparison ---
        bool operator==(const Vec2 &other) const = default;

        // --- Friend operators ---
        friend Vec2 operator*(float scalar, const Vec2 &vec);
        friend Vec2 operator/(float scalar, const Vec2 &vec);

        // --- Mathematical functions ---
        float Length() const;
        Vec2 Normalized() const;
        float Dot(const Vec2 &other) const;

        // --- Constants ---
        static const Vec2 Zero;
        static const Vec2 One;
    };

    inline constexpr Vec2 Vec2::Zero{0.0f, 0.0f};
    inline constexpr Vec2 Vec2::One{1.0f, 1.0f};
}