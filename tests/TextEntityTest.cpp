#include <gtest/gtest.h>
#include "Entity/TextEntity.h"
#include "Rendering/Renderer2D.h"
#include "Types/TypeRegistry.h"

using namespace Engine;

// Renderer2D's camera is a static, so each test sets the view it needs and puts the original back.
class TextEntityTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        m_originalCamera = Renderer2D::GetCamera();

        // 10 world units tall over 1200 by 800 pixels, so the view spans 15 by 10 units.
        Renderer2D::GetCamera().SetWindowSize(1200, 800);
        Renderer2D::GetCamera().SetPosition(Vec2(3.0f, -2.0f));
    }

    void TearDown() override
    {
        Renderer2D::GetCamera() = m_originalCamera;
    }

    // Sets a property the way a scene document does: from text, through the type registry.
    void SetProperty(TextEntity &label, const std::string &name, std::string_view text)
    {
        TypeRegistry &registry = TypeRegistry::Get();

        std::any value = registry.ParseProperty("TextEntity", name, text);
        ASSERT_TRUE(value.has_value()) << name << " did not parse";

        registry.SetProperty(&label, "TextEntity", name, value);
    }

    Camera2D m_originalCamera;
};

TEST_F(TextEntityTest, ScreenSpaceLabelTakesItsWorldPositionFromTheCamera)
{
    TextEntity label;
    SetProperty(label, "ScreenSpace", "true");
    SetProperty(label, "ScreenPosition", "300, 200");

    label.Update(0.0f);

    Vec2 expected = Renderer2D::GetCamera().ScreenToWorld(Vec2(300.0f, 200.0f));
    EXPECT_EQ(label.GetWorldPosition(), expected);
}

TEST_F(TextEntityTest, ScreenSpaceLabelStaysPutOnScreenWhileTheCameraMoves)
{
    TextEntity label;
    SetProperty(label, "ScreenSpace", "true");
    SetProperty(label, "ScreenPosition", "300, 200");
    label.Update(0.0f);
    Vec2 beforeTheMove = label.GetWorldPosition();

    Renderer2D::GetCamera().SetPosition(Vec2(10.0f, 6.0f));
    label.Update(0.0f);

    EXPECT_NE(label.GetWorldPosition(), beforeTheMove);

    Vec2 onScreen = Renderer2D::GetCamera().WorldToScreen(label.GetWorldPosition());
    EXPECT_NEAR(onScreen.x, 300.0f, 1e-3f);
    EXPECT_NEAR(onScreen.y, 200.0f, 1e-3f);
}

TEST_F(TextEntityTest, LabelStaysInWorldSpaceUnlessScreenSpaceIsSet)
{
    TextEntity label;
    SetProperty(label, "ScreenPosition", "300, 200");
    label.SetWorldPosition(Vec2(1.0f, 4.0f));

    label.Update(0.0f);

    EXPECT_EQ(label.GetWorldPosition(), Vec2(1.0f, 4.0f));
}
