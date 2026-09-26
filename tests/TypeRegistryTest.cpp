#include <gtest/gtest.h>
#include "Entity/Entity.h"
#include "Types/TypeRegistrationMacros.h"
#include "Types/TypeRegistry.h"

using namespace Engine;

// Registered here so the tests have a type with a property of every parsable kind, plus one
// pointer property that has no text form.
class RegistryTestEntity : public Engine::Entity
{
    DECLARE_TYPE(RegistryTestEntity, Entity)

    float m_speed{0.0f};
    int m_count{0};
    bool m_enabled{false};
    std::string m_label;
    Vec2 m_offset{};
    Color m_tint{};
    Engine::Entity *m_target{nullptr};

public:
    float GetSpeed() const { return m_speed; }
    int GetCount() const { return m_count; }
    bool IsEnabled() const { return m_enabled; }
    const std::string &GetLabel() const { return m_label; }
    Vec2 GetOffset() const { return m_offset; }
    Color GetTint() const { return m_tint; }
    Engine::Entity *GetTarget() const { return m_target; }
};

BEGIN_TYPE_REGISTER(RegistryTestEntity)
    REGISTER_PROPERTY(float, Speed, &RegistryTestEntity::m_speed)
    REGISTER_PROPERTY(int, Count, &RegistryTestEntity::m_count)
    REGISTER_PROPERTY(bool, Enabled, &RegistryTestEntity::m_enabled)
    REGISTER_PROPERTY(std::string, Label, &RegistryTestEntity::m_label)
    REGISTER_PROPERTY(Vec2, Offset, &RegistryTestEntity::m_offset)
    REGISTER_PROPERTY(Color, Tint, &RegistryTestEntity::m_tint)
    REGISTER_PROPERTY(Engine::Entity *, Target, &RegistryTestEntity::m_target)
END_TYPE_REGISTER()

class RegistryTestChild : public RegistryTestEntity
{
    DECLARE_TYPE(RegistryTestChild, RegistryTestEntity)

    float m_extra{0.0f};

public:
    float GetExtra() const { return m_extra; }
};

BEGIN_TYPE_REGISTER(RegistryTestChild)
    REGISTER_PROPERTY(float, Extra, &RegistryTestChild::m_extra)
END_TYPE_REGISTER()

namespace
{
    TypeRegistry &Registry() { return TypeRegistry::Get(); }
}

// --- Registration ---

TEST(TypeRegistryTest, TypesRegisterThemselves)
{
    EXPECT_TRUE(Registry().IsTypeRegistered("RegistryTestEntity"));
    EXPECT_TRUE(Registry().IsTypeRegistered("RegistryTestChild"));
    EXPECT_FALSE(Registry().IsTypeRegistered("NeverRegistered"));
}

// --- Property Lookup ---

TEST(TypeRegistryTest, HasPropertyFindsOwnProperties)
{
    EXPECT_TRUE(Registry().HasProperty("RegistryTestEntity", "Speed"));
    EXPECT_FALSE(Registry().HasProperty("RegistryTestEntity", "NoSuchProperty"));
    EXPECT_FALSE(Registry().HasProperty("NeverRegistered", "Speed"));
}

TEST(TypeRegistryTest, HasPropertyFindsInheritedProperties)
{
    EXPECT_TRUE(Registry().HasProperty("RegistryTestChild", "Extra"));
    EXPECT_TRUE(Registry().HasProperty("RegistryTestChild", "Speed"));
}

// --- ParseProperty ---

TEST(TypeRegistryTest, ParsePropertyReturnsTheValueAsItsRegisteredType)
{
    std::any speed = Registry().ParseProperty("RegistryTestEntity", "Speed", "12.5");
    ASSERT_TRUE(speed.has_value());
    EXPECT_FLOAT_EQ(std::any_cast<float>(speed), 12.5f);

    std::any offset = Registry().ParseProperty("RegistryTestEntity", "Offset", "0.4, 0.4");
    ASSERT_TRUE(offset.has_value());
    EXPECT_EQ(std::any_cast<Vec2>(offset), Vec2(0.4f, 0.4f));
}

TEST(TypeRegistryTest, ParsePropertyResolvesInheritedProperties)
{
    std::any speed = Registry().ParseProperty("RegistryTestChild", "Speed", "3");
    ASSERT_TRUE(speed.has_value());
    EXPECT_FLOAT_EQ(std::any_cast<float>(speed), 3.0f);
}

TEST(TypeRegistryTest, ParsePropertyIsEmptyForUnknownPropertiesAndTypes)
{
    EXPECT_FALSE(Registry().ParseProperty("RegistryTestEntity", "NoSuchProperty", "1").has_value());
    EXPECT_FALSE(Registry().ParseProperty("NeverRegistered", "Speed", "1").has_value());
}

TEST(TypeRegistryTest, ParsePropertyIsEmptyForPropertiesWithNoTextForm)
{
    EXPECT_TRUE(Registry().HasProperty("RegistryTestEntity", "Target"));
    EXPECT_FALSE(Registry().ParseProperty("RegistryTestEntity", "Target", "anything").has_value());
}

TEST(TypeRegistryTest, ParsePropertyIsEmptyWhenTheTextDoesNotParse)
{
    EXPECT_FALSE(Registry().ParseProperty("RegistryTestEntity", "Speed", "fast").has_value());
    EXPECT_FALSE(Registry().ParseProperty("RegistryTestEntity", "Offset", "1").has_value());
}

// --- Text To Entity ---

// Follows the path a scene file takes: parsing builds a PropertyMap, and the Scene applies the
// whole map later, through the SetProperties that already existed.
TEST(TypeRegistryTest, AParsedPropertyMapCanBeAppliedToAnEntity)
{
    const std::string type = "RegistryTestEntity";

    PropertyMap properties;
    properties["Speed"] = Registry().ParseProperty(type, "Speed", "9");
    properties["Count"] = Registry().ParseProperty(type, "Count", "4");
    properties["Enabled"] = Registry().ParseProperty(type, "Enabled", "true");
    properties["Label"] = Registry().ParseProperty(type, "Label", " hello ");
    properties["Offset"] = Registry().ParseProperty(type, "Offset", "1, 2");
    properties["Tint"] = Registry().ParseProperty(type, "Tint", "0.5, 0.5, 0.5");

    RegistryTestEntity entity;
    Registry().SetProperties(&entity, type, properties);

    EXPECT_FLOAT_EQ(entity.GetSpeed(), 9.0f);
    EXPECT_EQ(entity.GetCount(), 4);
    EXPECT_TRUE(entity.IsEnabled());
    EXPECT_EQ(entity.GetLabel(), "hello");
    EXPECT_EQ(entity.GetOffset(), Vec2(1.0f, 2.0f));
    EXPECT_FLOAT_EQ(entity.GetTint().a, 1.0f);
}

// How a loader tells "there is no such property" apart from "that text is not a valid value",
// so it can say which when it reports the line.
TEST(TypeRegistryTest, UnknownAndUnparsablePropertiesAreDistinguishable)
{
    const std::string type = "RegistryTestEntity";

    EXPECT_FALSE(Registry().HasProperty(type, "Wobble"));

    EXPECT_TRUE(Registry().HasProperty(type, "Speed"));
    EXPECT_FALSE(Registry().ParseProperty(type, "Speed", "quite fast").has_value());
}

TEST(TypeRegistryTest, SetPropertyReportsWhetherItFoundTheProperty)
{
    RegistryTestEntity entity;

    EXPECT_TRUE(Registry().SetProperty(&entity, "RegistryTestEntity", "Speed", std::any(5.0f)));
    EXPECT_FALSE(Registry().SetProperty(&entity, "RegistryTestEntity", "Nope", std::any(5.0f)));
}

// --- Factory ---

TEST(TypeRegistryTest, CreateBuildsRegisteredTypesByName)
{
    Entity *created = Registry().Create("RegistryTestEntity");
    ASSERT_NE(created, nullptr);
    EXPECT_NE(dynamic_cast<RegistryTestEntity *>(created), nullptr);
    delete created;

    EXPECT_EQ(Registry().Create("NeverRegistered"), nullptr);
}
