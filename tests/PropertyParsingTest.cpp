#include <gtest/gtest.h>
#include "Types/PropertyParsing.h"

using namespace Engine;

// --- Concept ---

static_assert(ParsableProperty<float>);
static_assert(ParsableProperty<int>);
static_assert(ParsableProperty<bool>);
static_assert(ParsableProperty<std::string>);
static_assert(ParsableProperty<Vec2>);
static_assert(ParsableProperty<Color>);

// A type with no PropertyParser specialization can't come from text.
struct Unparsable {};
static_assert(!ParsableProperty<Unparsable>);
static_assert(!ParsableProperty<float *>);

// --- float ---

TEST(PropertyParsing, ParsesFloats)
{
    EXPECT_FLOAT_EQ(*PropertyParser<float>::Parse("12"), 12.0f);
    EXPECT_FLOAT_EQ(*PropertyParser<float>::Parse("12.5"), 12.5f);
    EXPECT_FLOAT_EQ(*PropertyParser<float>::Parse("-0.3"), -0.3f);
    EXPECT_FLOAT_EQ(*PropertyParser<float>::Parse("  7  "), 7.0f);
}

TEST(PropertyParsing, RejectsFloatsWithTrailingJunk)
{
    EXPECT_FALSE(PropertyParser<float>::Parse("12x").has_value());
    EXPECT_FALSE(PropertyParser<float>::Parse("12 34").has_value());
    EXPECT_FALSE(PropertyParser<float>::Parse("abc").has_value());
    EXPECT_FALSE(PropertyParser<float>::Parse("").has_value());
    EXPECT_FALSE(PropertyParser<float>::Parse("   ").has_value());
}

// --- int ---

TEST(PropertyParsing, ParsesInts)
{
    EXPECT_EQ(*PropertyParser<int>::Parse("42"), 42);
    EXPECT_EQ(*PropertyParser<int>::Parse("-7"), -7);
    EXPECT_EQ(*PropertyParser<int>::Parse(" 0 "), 0);
}

TEST(PropertyParsing, RejectsIntsThatAreNotWholeNumbers)
{
    EXPECT_FALSE(PropertyParser<int>::Parse("4.5").has_value());
    EXPECT_FALSE(PropertyParser<int>::Parse("7cm").has_value());
    EXPECT_FALSE(PropertyParser<int>::Parse("").has_value());
}

// --- bool ---

TEST(PropertyParsing, ParsesBools)
{
    EXPECT_TRUE(*PropertyParser<bool>::Parse("true"));
    EXPECT_FALSE(*PropertyParser<bool>::Parse("false"));
    EXPECT_TRUE(*PropertyParser<bool>::Parse("  true  "));
}

TEST(PropertyParsing, RejectsOtherSpellingsOfBool)
{
    EXPECT_FALSE(PropertyParser<bool>::Parse("1").has_value());
    EXPECT_FALSE(PropertyParser<bool>::Parse("True").has_value());
    EXPECT_FALSE(PropertyParser<bool>::Parse("yes").has_value());
}

// --- string ---

TEST(PropertyParsing, ParsesStringsAndTrimsThem)
{
    EXPECT_EQ(*PropertyParser<std::string>::Parse("hello"), "hello");
    EXPECT_EQ(*PropertyParser<std::string>::Parse("  padded  "), "padded");
    EXPECT_EQ(*PropertyParser<std::string>::Parse("two words"), "two words");
    EXPECT_EQ(*PropertyParser<std::string>::Parse(""), "");
}

// --- Vec2 ---

TEST(PropertyParsing, ParsesVec2)
{
    EXPECT_EQ(*PropertyParser<Vec2>::Parse("0.4, 0.4"), Vec2(0.4f, 0.4f));
    EXPECT_EQ(*PropertyParser<Vec2>::Parse("1,2"), Vec2(1.0f, 2.0f));
    EXPECT_EQ(*PropertyParser<Vec2>::Parse(" -3 , 4 "), Vec2(-3.0f, 4.0f));
}

TEST(PropertyParsing, RejectsVec2WithWrongComponentCount)
{
    EXPECT_FALSE(PropertyParser<Vec2>::Parse("1").has_value());
    EXPECT_FALSE(PropertyParser<Vec2>::Parse("1,2,3").has_value());
    EXPECT_FALSE(PropertyParser<Vec2>::Parse("1,").has_value());
}

// --- Color ---

TEST(PropertyParsing, ParsesColorWithAlpha)
{
    Color color = *PropertyParser<Color>::Parse("1, 0, 0.5, 0.25");
    EXPECT_FLOAT_EQ(color.r, 1.0f);
    EXPECT_FLOAT_EQ(color.g, 0.0f);
    EXPECT_FLOAT_EQ(color.b, 0.5f);
    EXPECT_FLOAT_EQ(color.a, 0.25f);
}

TEST(PropertyParsing, ColorAlphaDefaultsToOpaque)
{
    Color color = *PropertyParser<Color>::Parse("0.3, 0.3, 0.3");
    EXPECT_FLOAT_EQ(color.r, 0.3f);
    EXPECT_FLOAT_EQ(color.a, 1.0f);
}

TEST(PropertyParsing, RejectsColorWithWrongComponentCount)
{
    EXPECT_FALSE(PropertyParser<Color>::Parse("1, 0").has_value());
    EXPECT_FALSE(PropertyParser<Color>::Parse("1, 0, 0, 1, 0").has_value());
}

// --- MakePropertyParser ---

TEST(PropertyParsing, MakePropertyParserYieldsNullForUnparsableTypes)
{
    EXPECT_EQ(MakePropertyParser<Unparsable>(), nullptr);
    EXPECT_NE(MakePropertyParser<float>(), nullptr);
}

TEST(PropertyParsing, MakePropertyParserBoxesTheParsedValue)
{
    std::any value = MakePropertyParser<Vec2>()("2, 3");

    ASSERT_TRUE(value.has_value());
    EXPECT_EQ(std::any_cast<Vec2>(value), Vec2(2.0f, 3.0f));
}

TEST(PropertyParsing, MakePropertyParserYieldsAnEmptyAnyOnFailure)
{
    EXPECT_FALSE(MakePropertyParser<Vec2>()("not a vector").has_value());
}
