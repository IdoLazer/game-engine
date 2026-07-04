#include <gtest/gtest.h>
#include "Math/Rect.h"

using namespace Engine;

// --- Construction ---

TEST(RectTest, DefaultConstructsToZero)
{
    Rect r;
    EXPECT_EQ(r.center, Vec2(0.0f, 0.0f));
    EXPECT_EQ(r.halfExtents, Vec2(0.0f, 0.0f));
}

TEST(RectTest, ConstructsWithCenterAndHalfExtents)
{
    Rect r(Vec2(1.0f, 2.0f), Vec2(3.0f, 4.0f));
    EXPECT_EQ(r.center, Vec2(1.0f, 2.0f));
    EXPECT_EQ(r.halfExtents, Vec2(3.0f, 4.0f));
}

// --- Min / Max ---

TEST(RectTest, MinReturnsCenterMinusHalfExtents)
{
    Rect r(Vec2(5.0f, 5.0f), Vec2(2.0f, 1.0f));
    EXPECT_EQ(r.Min(), Vec2(3.0f, 4.0f));
}

TEST(RectTest, MaxReturnsCenterPlusHalfExtents)
{
    Rect r(Vec2(5.0f, 5.0f), Vec2(2.0f, 1.0f));
    EXPECT_EQ(r.Max(), Vec2(7.0f, 6.0f));
}

// --- Contains ---

TEST(RectTest, ContainsPointInside)
{
    Rect r(Vec2(0.0f, 0.0f), Vec2(1.0f, 1.0f));
    EXPECT_TRUE(r.Contains(Vec2(0.5f, -0.5f)));
}

TEST(RectTest, ContainsPointOutside)
{
    Rect r(Vec2(0.0f, 0.0f), Vec2(1.0f, 1.0f));
    EXPECT_FALSE(r.Contains(Vec2(2.0f, 0.0f)));
}

TEST(RectTest, ContainsPointOnBoundaryIsInclusive)
{
    Rect r(Vec2(0.0f, 0.0f), Vec2(1.0f, 1.0f));
    EXPECT_TRUE(r.Contains(Vec2(1.0f, 1.0f)));
    EXPECT_TRUE(r.Contains(Vec2(-1.0f, -1.0f)));
}

// --- Overlaps ---

TEST(RectTest, OverlapsWhenIntersecting)
{
    Rect a(Vec2(0.0f, 0.0f), Vec2(1.0f, 1.0f));
    Rect b(Vec2(1.5f, 0.0f), Vec2(1.0f, 1.0f));
    EXPECT_TRUE(a.Overlaps(b));
    EXPECT_TRUE(b.Overlaps(a));
}

TEST(RectTest, DoesNotOverlapWhenDisjoint)
{
    Rect a(Vec2(0.0f, 0.0f), Vec2(1.0f, 1.0f));
    Rect b(Vec2(5.0f, 5.0f), Vec2(1.0f, 1.0f));
    EXPECT_FALSE(a.Overlaps(b));
}

TEST(RectTest, OverlapsWhenTouchingAtEdge)
{
    Rect a(Vec2(0.0f, 0.0f), Vec2(1.0f, 1.0f));
    Rect b(Vec2(2.0f, 0.0f), Vec2(1.0f, 1.0f));
    EXPECT_TRUE(a.Overlaps(b));
}

// --- Expanded ---

TEST(RectTest, ExpandedKeepsCenterAndGrowsHalfExtents)
{
    Rect r(Vec2(1.0f, 2.0f), Vec2(1.0f, 1.0f));
    Rect expanded = r.Expanded(Vec2(0.5f, 0.25f));
    EXPECT_EQ(expanded.center, Vec2(1.0f, 2.0f));
    EXPECT_EQ(expanded.halfExtents, Vec2(1.5f, 1.25f));
}
