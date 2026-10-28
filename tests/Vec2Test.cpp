#include <gtest/gtest.h>
#include "Math/Vec2.h"

#include <sstream>

using namespace Engine;

// --- Construction ---

TEST(Vec2Test, DefaultConstructsToZero)
{
    Vec2 v;
    EXPECT_FLOAT_EQ(v.x, 0.0f);
    EXPECT_FLOAT_EQ(v.y, 0.0f);
}

TEST(Vec2Test, ConstructsWithFloats)
{
    Vec2 v(3.0f, 4.0f);
    EXPECT_FLOAT_EQ(v.x, 3.0f);
    EXPECT_FLOAT_EQ(v.y, 4.0f);
}

TEST(Vec2Test, ConstructsWithInts)
{
    Vec2 v(3, 4);
    EXPECT_FLOAT_EQ(v.x, 3.0f);
    EXPECT_FLOAT_EQ(v.y, 4.0f);
}

// --- Arithmetic ---

TEST(Vec2Test, Addition)
{
    Vec2 a(1.0f, 2.0f);
    Vec2 b(3.0f, 4.0f);
    Vec2 result = a + b;
    EXPECT_EQ(result, Vec2(4.0f, 6.0f));
}

TEST(Vec2Test, Subtraction)
{
    Vec2 a(5.0f, 7.0f);
    Vec2 b(2.0f, 3.0f);
    EXPECT_EQ(a - b, Vec2(3.0f, 4.0f));
}

TEST(Vec2Test, Negation)
{
    Vec2 v(3.0f, -4.0f);
    EXPECT_EQ(-v, Vec2(-3.0f, 4.0f));
}

TEST(Vec2Test, ScalarMultiply)
{
    Vec2 v(2.0f, 3.0f);
    EXPECT_EQ(v * 2.0f, Vec2(4.0f, 6.0f));
    EXPECT_EQ(2.0f * v, Vec2(4.0f, 6.0f));
}

TEST(Vec2Test, ScalarDivide)
{
    Vec2 v(6.0f, 4.0f);
    EXPECT_EQ(v / 2.0f, Vec2(3.0f, 2.0f));
}

// --- Compound assignment ---

TEST(Vec2Test, PlusEquals)
{
    Vec2 v(1.0f, 2.0f);
    v += Vec2(3.0f, 4.0f);
    EXPECT_EQ(v, Vec2(4.0f, 6.0f));
}

TEST(Vec2Test, MinusEquals)
{
    Vec2 v(5.0f, 7.0f);
    v -= Vec2(2.0f, 3.0f);
    EXPECT_EQ(v, Vec2(3.0f, 4.0f));
}

TEST(Vec2Test, TimesEquals)
{
    Vec2 v(2.0f, 3.0f);
    v *= 3.0f;
    EXPECT_EQ(v, Vec2(6.0f, 9.0f));
}

TEST(Vec2Test, DivideEquals)
{
    Vec2 v(6.0f, 9.0f);
    v /= 3.0f;
    EXPECT_EQ(v, Vec2(2.0f, 3.0f));
}

// --- Stream output ---

TEST(Vec2Test, StreamOutput)
{
    std::ostringstream stream;
    stream << Vec2(1, 2);
    EXPECT_EQ(stream.str(), "(1, 2)");
}

// --- Math functions ---

TEST(Vec2Test, Length)
{
    Vec2 v(3.0f, 4.0f);
    EXPECT_FLOAT_EQ(v.Length(), 5.0f);
}

TEST(Vec2Test, Normalized)
{
    Vec2 v(3.0f, 4.0f);
    Vec2 n = v.Normalized();
    EXPECT_FLOAT_EQ(n.x, 0.6f);
    EXPECT_FLOAT_EQ(n.y, 0.8f);
}

TEST(Vec2Test, NormalizedZeroVector)
{
    Vec2 v;
    Vec2 n = v.Normalized();
    EXPECT_EQ(n, Vec2::Zero);
}

TEST(Vec2Test, DotProduct)
{
    Vec2 a(1.0f, 0.0f);
    Vec2 b(0.0f, 1.0f);
    EXPECT_FLOAT_EQ(a.Dot(b), 0.0f);

    EXPECT_FLOAT_EQ(Vec2(2.0f, 3.0f).Dot(Vec2(4.0f, 5.0f)), 23.0f);
}

TEST(Vec2Test, MoveTowardsShortStep)
{
    Vec2 start(1.0f, 1.0f);
    Vec2 target(4.0f, 5.0f);
    Vec2 moved = start.MoveTowards(target, 2.5f);
    EXPECT_FLOAT_EQ(moved.x, 2.5f);
    EXPECT_FLOAT_EQ(moved.y, 3.0f);
}

TEST(Vec2Test, MoveTowardsLongStepLandsOnTarget)
{
    Vec2 start(1.0f, 1.0f);
    Vec2 target(4.0f, 5.0f);
    EXPECT_EQ(start.MoveTowards(target, 10.0f), target);
}

TEST(Vec2Test, MoveTowardsFromTargetReturnsTarget)
{
    Vec2 target(2.0f, 3.0f);
    EXPECT_EQ(target.MoveTowards(target, 1.0f), target);
}

TEST(Vec2Test, MoveTowardsZeroDistanceReturnsStart)
{
    Vec2 start(1.0f, 1.0f);
    Vec2 target(4.0f, 5.0f);
    EXPECT_EQ(start.MoveTowards(target, 0.0f), start);
}

// --- Constants ---

TEST(Vec2Test, Constants)
{
    EXPECT_EQ(Vec2::Zero, Vec2(0.0f, 0.0f));
    EXPECT_EQ(Vec2::One, Vec2(1.0f, 1.0f));
}
