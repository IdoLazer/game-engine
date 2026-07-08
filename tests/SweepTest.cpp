#include <gtest/gtest.h>
#include "Collision/Sweep.h"

using namespace Engine;

namespace
{
    // A 1x1 unit "tile" centered at (5,5), spanning [4.5,5.5] x [4.5,5.5] - stands in for a
    // static solid cell in every test below.
    const Rect kTarget(Vec2(5.0f, 5.0f), Vec2(0.5f, 0.5f));
}

// --- Head-on hits (also exercises the zero-delta-on-the-other-axis branch) ---

TEST(SweepRectVsRectTest, HitFromLeft)
{
    Rect mover(Vec2(2.0f, 5.0f), Vec2(0.5f, 0.5f));
    SweepHit hit = SweepRectVsRect(mover, Vec2(10.0f, 0.0f), kTarget);

    EXPECT_TRUE(hit.hit);
    EXPECT_FLOAT_EQ(hit.t, 0.2f);
    EXPECT_EQ(hit.normal, Vec2(-1.0f, 0.0f));
}

TEST(SweepRectVsRectTest, HitFromRight)
{
    Rect mover(Vec2(8.0f, 5.0f), Vec2(0.5f, 0.5f));
    SweepHit hit = SweepRectVsRect(mover, Vec2(-10.0f, 0.0f), kTarget);

    EXPECT_TRUE(hit.hit);
    EXPECT_FLOAT_EQ(hit.t, 0.2f);
    EXPECT_EQ(hit.normal, Vec2(1.0f, 0.0f));
}

TEST(SweepRectVsRectTest, HitFromBelow)
{
    Rect mover(Vec2(5.0f, 2.0f), Vec2(0.5f, 0.5f));
    SweepHit hit = SweepRectVsRect(mover, Vec2(0.0f, 10.0f), kTarget);

    EXPECT_TRUE(hit.hit);
    EXPECT_FLOAT_EQ(hit.t, 0.2f);
    EXPECT_EQ(hit.normal, Vec2(0.0f, -1.0f));
}

TEST(SweepRectVsRectTest, HitFromAbove)
{
    Rect mover(Vec2(5.0f, 8.0f), Vec2(0.5f, 0.5f));
    SweepHit hit = SweepRectVsRect(mover, Vec2(0.0f, -10.0f), kTarget);

    EXPECT_TRUE(hit.hit);
    EXPECT_FLOAT_EQ(hit.t, 0.2f);
    EXPECT_EQ(hit.normal, Vec2(0.0f, 1.0f));
}

// --- Misses ---

TEST(SweepRectVsRectTest, MissesWhenDeltaTooShortToReach)
{
    Rect mover(Vec2(2.0f, 5.0f), Vec2(0.5f, 0.5f));
    SweepHit hit = SweepRectVsRect(mover, Vec2(1.0f, 0.0f), kTarget);

    EXPECT_FALSE(hit.hit);
}

TEST(SweepRectVsRectTest, MissesWhenMovingAway)
{
    Rect mover(Vec2(2.0f, 5.0f), Vec2(0.5f, 0.5f));
    SweepHit hit = SweepRectVsRect(mover, Vec2(-5.0f, 0.0f), kTarget);

    EXPECT_FALSE(hit.hit);
}

TEST(SweepRectVsRectTest, MissesWhenParallelHorizontalMotionIsNotAligned)
{
    // Pure horizontal motion, but far away on the Y axis - never in the same row.
    Rect mover(Vec2(2.0f, 20.0f), Vec2(0.5f, 0.5f));
    SweepHit hit = SweepRectVsRect(mover, Vec2(10.0f, 0.0f), kTarget);

    EXPECT_FALSE(hit.hit);
}

TEST(SweepRectVsRectTest, MissesWhenParallelVerticalMotionIsNotAligned)
{
    // Pure vertical motion, but far away on the X axis - never in the same column.
    Rect mover(Vec2(20.0f, 2.0f), Vec2(0.5f, 0.5f));
    SweepHit hit = SweepRectVsRect(mover, Vec2(0.0f, 10.0f), kTarget);

    EXPECT_FALSE(hit.hit);
}

// --- Resting contact ---

TEST(SweepRectVsRectTest, RestingOnTopDoesNotBlockHorizontalSliding)
{
    // Mover's bottom edge (y = 5.5) sits exactly flush on kTarget's top edge (y = 5.5).
    // Sliding purely horizontally must not be blocked by the ground it's merely 
    // touching from above.
    Rect mover(Vec2(5.0f, 6.0f), Vec2(0.5f, 0.5f));
    SweepHit hit = SweepRectVsRect(mover, Vec2(3.0f, 0.0f), kTarget);

    EXPECT_FALSE(hit.hit);
}

TEST(SweepRectVsRectTest, TouchingSideDoesNotBlockVerticalSliding)
{
    // Mover's right edge (x = 4.5) sits exactly flush on kTarget's left edge (x = 4.5).
    // Sliding purely vertically must not be blocked by the wall it's merely 
    // touching from the side.
    Rect mover(Vec2(4.0f, 5.0f), Vec2(0.5f, 0.5f));
    SweepHit hit = SweepRectVsRect(mover, Vec2(0.0f, 3.0f), kTarget);

    EXPECT_FALSE(hit.hit);
}

// --- Moving directly away from an exact touch ---

TEST(SweepRectVsRectTest, JumpingStraightUpFromExactTouchIsAMiss)
{
    // Mover's bottom edge is exactly flush with kTarget's top edge. 
    // Moving straight up (away) must not be blocked by the ground it's leaving.
    Rect mover(Vec2(5.0f, 6.0f), Vec2(0.5f, 0.5f));
    SweepHit hit = SweepRectVsRect(mover, Vec2(0.0f, 3.0f), kTarget);

    EXPECT_FALSE(hit.hit);
}

TEST(SweepRectVsRectTest, PushingStraightDownFromExactTouchStillBlocks)
{
    // Same exact-touch setup as above, but continuing to push down (further into the ground,
    // not away from it) must still be blocked, at t = 0.
    Rect mover(Vec2(5.0f, 6.0f), Vec2(0.5f, 0.5f));
    SweepHit hit = SweepRectVsRect(mover, Vec2(0.0f, -3.0f), kTarget);

    EXPECT_TRUE(hit.hit);
    EXPECT_FLOAT_EQ(hit.t, 0.0f);
}

TEST(SweepRectVsRectTest, BackingAwayFromExactSideTouchIsAMiss)
{
    // Mover's right edge is exactly flush with kTarget's left edge.
    // Moving left (away) must not be blocked by that wall.
    Rect mover(Vec2(4.0f, 5.0f), Vec2(0.5f, 0.5f));
    SweepHit hit = SweepRectVsRect(mover, Vec2(-3.0f, 0.0f), kTarget);

    EXPECT_FALSE(hit.hit);
}

TEST(SweepRectVsRectTest, PushingFurtherIntoExactTouchStillBlocks)
{
    // Same exact-touch setup as above, but continuing to push right (further into the wall,
    // not away from it) must still be blocked, at t = 0.
    Rect mover(Vec2(4.0f, 5.0f), Vec2(0.5f, 0.5f));
    SweepHit hit = SweepRectVsRect(mover, Vec2(3.0f, 0.0f), kTarget);

    EXPECT_TRUE(hit.hit);
    EXPECT_FLOAT_EQ(hit.t, 0.0f);
}

// --- Zero half-extents: a moving Rect with no size is just a ray ---

TEST(SweepRectVsRectTest, ZeroHalfExtentsBehavesAsRay)
{
    Rect ray(Vec2(0.0f, 0.0f), Vec2(0.0f, 0.0f));
    Rect wall(Vec2(5.0f, 0.0f), Vec2(1.0f, 1.0f)); // spans x:[4,6], y:[-1,1]

    SweepHit hit = SweepRectVsRect(ray, Vec2(10.0f, 0.0f), wall);

    EXPECT_TRUE(hit.hit);
    EXPECT_FLOAT_EQ(hit.t, 0.4f); // enters at x=4, i.e. 4/10 of the way along
    EXPECT_EQ(hit.normal, Vec2(-1.0f, 0.0f));
}

// --- Already overlapping ---

TEST(SweepRectVsRectTest, AlreadyOverlappingHitsImmediatelyAtZero)
{
    Rect mover(Vec2(5.0f, 5.0f), Vec2(0.1f, 0.1f)); // starts inside kTarget
    SweepHit hit = SweepRectVsRect(mover, Vec2(0.0f, 0.0f), kTarget);

    EXPECT_TRUE(hit.hit);
    EXPECT_FLOAT_EQ(hit.t, 0.0f);
}

TEST(SweepRectVsRectTest, AlreadyOverlappingHitsImmediatelyAtZeroWithNonZeroVelocity)
{
    Rect mover(Vec2(5.0f, 5.0f), Vec2(0.1f, 0.1f)); // starts inside kTarget
    SweepHit hit = SweepRectVsRect(mover, Vec2(10.0f, 10.0f), kTarget);

    EXPECT_TRUE(hit.hit);
    EXPECT_FLOAT_EQ(hit.t, 0.0f);
}


// --- Tunneling: the headline case a destination-only check would miss ---

TEST(SweepRectVsRectTest, CatchesThinWallAtHighSpeed)
{
    // A thin vertical wall (0.2 units wide) at x=10. The mover travels from x=0 to x=30 in a
    // single frame - its destination is far past the wall, so a "check only the final position"
    // test would report no collision at all. The sweep must still catch it.
    Rect thinWall(Vec2(10.0f, 0.0f), Vec2(0.1f, 5.0f));
    Rect mover(Vec2(0.0f, 0.0f), Vec2(0.2f, 0.2f));

    SweepHit hit = SweepRectVsRect(mover, Vec2(30.0f, 0.0f), thinWall);

    EXPECT_TRUE(hit.hit);
    EXPECT_LT(hit.t, 1.0f);
    EXPECT_EQ(hit.normal, Vec2(-1.0f, 0.0f));
}

// --- Diagonal approach to a corner ---

TEST(SweepRectVsRectTest, DiagonalApproachResolvesAtCornerBeforePenetration)
{
    // A diagonal approach where both axes reach the target's expanded corner at the same
    // time - the exact scenario a separated single-axis sweep can miss (each axis alone, using
    // a stale cross-axis position, lets the mover tunnel through to a fully embedded position).
    // A combined 2D sweep must catch it.
    Rect tile(Vec2(10.0f, 10.0f), Vec2(0.5f, 0.5f)); // spans x:[9.5,10.5], y:[9.5,10.5]
    Rect mover(Vec2(9.0f, 9.0f), Vec2(0.2f, 0.2f)); // spans x:[8.8,9.2], y:[8.8,9.2]

    // Move to (10.0, 10.0) - the corner of the tile's expanded bounds (9.5, 9.5). The mover's final
    // position would be fully embedded in the tile, but the sweep must stop it at the corner instead.
    SweepHit hit = SweepRectVsRect(mover, Vec2(1.0f, 1.0f), tile);

    EXPECT_TRUE(hit.hit);
    EXPECT_NEAR(hit.t, 0.3f, 1e-6f); // 0.3 = (9.5 - 9.2) / 1.0
    EXPECT_EQ(hit.normal, Vec2(0.0f, -1.0f)); // vertical axis is the first to reach the expanded corner, so it wins the tie-breaker

    Vec2 restingCenter = mover.center + Vec2(1.0f, 1.0f) * hit.t;
    Rect restingBox(restingCenter, mover.halfExtents);
    EXPECT_NEAR(restingBox.Max().x, tile.Min().x, 1e-6f); // flush, not embedded
    EXPECT_NEAR(restingBox.Max().y, tile.Min().y, 1e-6f);
}
