#include <gtest/gtest.h>
#include "Rendering/Camera2D.h"

using namespace Engine;

class Camera2DTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        camera.SetWindowSize(1200, 800);
    }

    // The world height is given and the width follows the window's aspect ratio:
    // 10 * 1200 / 800 = 15, so the view spans 15 by 10 units.
    Camera2D camera{10.0f};
};

// --- Default position ---

TEST_F(Camera2DTest, DefaultPositionPutsTheWorldOriginAtTheCenter)
{
    Vec2 openGL = camera.WorldToOpenGL(Vec2::Zero);
    EXPECT_FLOAT_EQ(openGL.x, 0.0f);
    EXPECT_FLOAT_EQ(openGL.y, 0.0f);

    Vec2 screen = camera.WorldToScreen(Vec2::Zero);
    EXPECT_FLOAT_EQ(screen.x, 600.0f);
    EXPECT_FLOAT_EQ(screen.y, 400.0f);
}

TEST_F(Camera2DTest, ViewCornerMapsToOpenGLCorner)
{
    // Half the view is 7.5 wide and 5 tall.
    Vec2 openGL = camera.WorldToOpenGL(Vec2(7.5f, 5.0f));
    EXPECT_FLOAT_EQ(openGL.x, 1.0f);
    EXPECT_FLOAT_EQ(openGL.y, 1.0f);
}

// --- SetPosition ---

TEST_F(Camera2DTest, PositionBecomesTheCenterOfTheView)
{
    Vec2 position(3.0f, -2.0f);
    camera.SetPosition(position);

    EXPECT_EQ(camera.GetPosition(), position);

    Vec2 openGL = camera.WorldToOpenGL(position);
    EXPECT_FLOAT_EQ(openGL.x, 0.0f);
    EXPECT_FLOAT_EQ(openGL.y, 0.0f);

    Vec2 screen = camera.WorldToScreen(position);
    EXPECT_FLOAT_EQ(screen.x, 600.0f);
    EXPECT_FLOAT_EQ(screen.y, 400.0f);
}

TEST_F(Camera2DTest, ScreenToWorldIsRelativeToThePosition)
{
    Vec2 position(3.0f, -2.0f);
    camera.SetPosition(position);

    Vec2 center = camera.ScreenToWorld(Vec2(600.0f, 400.0f));
    EXPECT_FLOAT_EQ(center.x, position.x);
    EXPECT_FLOAT_EQ(center.y, position.y);

    // Screen y grows downward, so the top-left pixel is above the center in world space.
    Vec2 topLeft = camera.ScreenToWorld(Vec2(0.0f, 0.0f));
    EXPECT_FLOAT_EQ(topLeft.x, position.x - 7.5f);
    EXPECT_FLOAT_EQ(topLeft.y, position.y + 5.0f);
}

// --- Round trip ---

TEST_F(Camera2DTest, WorldToScreenThenScreenToWorldReturnsTheOriginalPoint)
{
    camera.SetPosition(Vec2(3.0f, -2.0f));

    Vec2 original(1.25f, 2.5f);
    Vec2 roundTrip = camera.ScreenToWorld(camera.WorldToScreen(original));
    EXPECT_NEAR(roundTrip.x, original.x, 1e-4f);
    EXPECT_NEAR(roundTrip.y, original.y, 1e-4f);
}
