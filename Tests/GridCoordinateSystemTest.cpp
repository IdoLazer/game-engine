#include <gtest/gtest.h>
#include "World/GridCoordinateSystem.h"

using namespace Engine;

// Standard test grid: 8x8, cell size 1.0, centered at origin
class GridCoordTest : public ::testing::Test
{
protected:
    // 8x8 grid, cellSize=1, centered at origin
    // topLeft = (-4, 4), bottomRight = (4, -4)
    GridCoordinateSystem grid{1.0f, Vec2{8, 8}};
};

// --- GridToWorld ---

TEST_F(GridCoordTest, CellZeroMapsToTopLeftCellCenter)
{
    Vec2 world = grid.GridToWorld(Vec2{0, 0});
    EXPECT_FLOAT_EQ(world.x, -3.5f);
    EXPECT_FLOAT_EQ(world.y, 3.5f);
}

TEST_F(GridCoordTest, LastCellMapsToBottomRightCellCenter)
{
    Vec2 world = grid.GridToWorld(Vec2{7, 7});
    EXPECT_FLOAT_EQ(world.x, 3.5f);
    EXPECT_FLOAT_EQ(world.y, -3.5f);
}

TEST_F(GridCoordTest, MiddleCellMapsToNearOrigin)
{
    // Cell (3,3) center with 8x8 grid at origin
    Vec2 world = grid.GridToWorld(Vec2{3, 3});
    EXPECT_FLOAT_EQ(world.x, -0.5f);
    EXPECT_FLOAT_EQ(world.y, 0.5f);
}

// --- WorldToGrid ---

TEST_F(GridCoordTest, WorldToGridIsInverseOfGridToWorld)
{
    Vec2 original{3, 5};
    Vec2 world = grid.GridToWorld(original);
    Vec2 roundTrip = grid.WorldToGrid(world);
    EXPECT_FLOAT_EQ(roundTrip.x, original.x);
    EXPECT_FLOAT_EQ(roundTrip.y, original.y);
}

TEST_F(GridCoordTest, RoundTripAtOriginCell)
{
    Vec2 original{0, 0};
    Vec2 roundTrip = grid.WorldToGrid(grid.GridToWorld(original));
    EXPECT_FLOAT_EQ(roundTrip.x, original.x);
    EXPECT_FLOAT_EQ(roundTrip.y, original.y);
}

TEST_F(GridCoordTest, RoundTripAtLastCell)
{
    Vec2 original{7, 7};
    Vec2 roundTrip = grid.WorldToGrid(grid.GridToWorld(original));
    EXPECT_FLOAT_EQ(roundTrip.x, original.x);
    EXPECT_FLOAT_EQ(roundTrip.y, original.y);
}

TEST_F(GridCoordTest, RoundTripWithFractionalPosition)
{
    Vec2 original{2.3f, 5.7f};
    Vec2 roundTrip = grid.WorldToGrid(grid.GridToWorld(original));
    EXPECT_NEAR(roundTrip.x, original.x, 1e-5f);
    EXPECT_NEAR(roundTrip.y, original.y, 1e-5f);
}

// --- GetCellFromGridPosition ---

TEST_F(GridCoordTest, CellCenterReturnsCorrectCell)
{
    EXPECT_EQ(grid.GetCellFromGridPosition(Vec2{0, 0}), Vec2(0, 0));
    EXPECT_EQ(grid.GetCellFromGridPosition(Vec2{3, 4}), Vec2(3, 4));
    EXPECT_EQ(grid.GetCellFromGridPosition(Vec2{7, 7}), Vec2(7, 7));
}

TEST_F(GridCoordTest, SlightlyOffCenterStillReturnsCorrectCell)
{
    // Just inside cell (2, 3) — offset from center but within the cell
    EXPECT_EQ(grid.GetCellFromGridPosition(Vec2{2.4f, 3.4f}), Vec2(2, 3));
    EXPECT_EQ(grid.GetCellFromGridPosition(Vec2{2.0f, 3.0f}), Vec2(2, 3));
    EXPECT_EQ(grid.GetCellFromGridPosition(Vec2{1.6f, 2.6f}), Vec2(2, 3));
}

TEST_F(GridCoordTest, WorldToGridThenCellMatchesExpected)
{
    // Convert cell center to world and back, then get cell
    Vec2 world = grid.GridToWorld(Vec2{4, 5});
    Vec2 gridPos = grid.WorldToGrid(world);
    Vec2 cell = grid.GetCellFromGridPosition(gridPos);
    EXPECT_EQ(cell, Vec2(4, 5));
}

// --- IsInBounds ---

TEST_F(GridCoordTest, CellCentersAreInBounds)
{
    EXPECT_TRUE(grid.IsInBounds(Vec2{0, 0}));
    EXPECT_TRUE(grid.IsInBounds(Vec2{7, 7}));
    EXPECT_TRUE(grid.IsInBounds(Vec2{3, 4}));
}

TEST_F(GridCoordTest, TopLeftCornerIsInBounds)
{
    // Top-left corner of cell (0,0) is at grid position (-0.5, -0.5)
    EXPECT_TRUE(grid.IsInBounds(Vec2{-0.5f, -0.5f}));
}

TEST_F(GridCoordTest, JustOutsideGridIsOutOfBounds)
{
    EXPECT_FALSE(grid.IsInBounds(Vec2{-0.51f, 0.0f}));
    EXPECT_FALSE(grid.IsInBounds(Vec2{0.0f, -0.51f}));
    EXPECT_FALSE(grid.IsInBounds(Vec2{7.5f, 0.0f}));
    EXPECT_FALSE(grid.IsInBounds(Vec2{0.0f, 7.5f}));
}

TEST_F(GridCoordTest, NegativePositionsOutOfBounds)
{
    EXPECT_FALSE(grid.IsInBounds(Vec2{-1.0f, 0.0f}));
    EXPECT_FALSE(grid.IsInBounds(Vec2{0.0f, -1.0f}));
}

TEST_F(GridCoordTest, BeyondGridCountOutOfBounds)
{
    EXPECT_FALSE(grid.IsInBounds(Vec2{8.0f, 0.0f}));
    EXPECT_FALSE(grid.IsInBounds(Vec2{0.0f, 8.0f}));
}

// --- Non-origin grid ---

TEST(GridCoordOffsetTest, OffsetGridRoundTrips)
{
    // Grid centered at (10, 5)
    GridCoordinateSystem offsetGrid{2.0f, Vec2{4, 4}, Vec2{10, 5}};

    Vec2 original{1, 2};
    Vec2 roundTrip = offsetGrid.WorldToGrid(offsetGrid.GridToWorld(original));
    EXPECT_NEAR(roundTrip.x, original.x, 1e-5f);
    EXPECT_NEAR(roundTrip.y, original.y, 1e-5f);
}

TEST(GridCoordOffsetTest, OffsetGridCellZeroPosition)
{
    // Grid centered at (10, 5), cellSize=2, 4x4
    // worldW=8, worldH=8, topLeft=(6, 9)
    // Cell (0,0) center = (6 + 0.5*2, 9 - 0.5*2) = (7, 8)
    GridCoordinateSystem offsetGrid{2.0f, Vec2{4, 4}, Vec2{10, 5}};

    Vec2 world = offsetGrid.GridToWorld(Vec2{0, 0});
    EXPECT_FLOAT_EQ(world.x, 7.0f);
    EXPECT_FLOAT_EQ(world.y, 8.0f);
}

// --- Different cell sizes ---

TEST(GridCoordCellSizeTest, LargeCellSizeRoundTrips)
{
    GridCoordinateSystem grid{50.0f, Vec2{3, 3}};

    Vec2 original{1.25f, 2.75f};
    Vec2 roundTrip = grid.WorldToGrid(grid.GridToWorld(original));
    EXPECT_NEAR(roundTrip.x, original.x, 1e-4f);
    EXPECT_NEAR(roundTrip.y, original.y, 1e-4f);
}

TEST(GridCoordCellSizeTest, SmallCellSizeRoundTrips)
{
    GridCoordinateSystem grid{0.01f, Vec2{100, 100}};

    Vec2 original{50, 50};
    Vec2 roundTrip = grid.WorldToGrid(grid.GridToWorld(original));
    EXPECT_NEAR(roundTrip.x, original.x, 1e-3f);
    EXPECT_NEAR(roundTrip.y, original.y, 1e-3f);
}
