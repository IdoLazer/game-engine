#include <gtest/gtest.h>
#include "Entity/TextEntity.h"
#include "Types/TypeRegistry.h"

using namespace Engine;

namespace
{
    // Sets a property the way a scene document does: from text, through the type registry.
    void SetProperty(TextEntity &label, const std::string &name, std::string_view text)
    {
        TypeRegistry &registry = TypeRegistry::Get();

        std::any value = registry.ParseProperty("TextEntity", name, text);
        ASSERT_TRUE(value.has_value()) << name << " did not parse";

        registry.SetProperty(&label, "TextEntity", name, value);
    }
}

TEST(TextEntityTest, ScreenSpaceLeavesTheWorldPositionAlone)
{
    TextEntity label;
    label.SetWorldPosition(Vec2(1.0f, 4.0f));

    SetProperty(label, "ScreenSpace", "true");
    SetProperty(label, "ScreenPosition", "300, 200");

    EXPECT_EQ(label.GetWorldPosition(), Vec2(1.0f, 4.0f));
    EXPECT_EQ(label.GetScreenPosition(), Vec2(300.0f, 200.0f));
}

TEST(TextEntityTest, SettingTheWorldPositionLeavesTheScreenPositionAlone)
{
    TextEntity label;
    SetProperty(label, "ScreenPosition", "300, 200");

    label.SetWorldPosition(Vec2(1.0f, 4.0f));

    EXPECT_EQ(label.GetScreenPosition(), Vec2(300.0f, 200.0f));
}
