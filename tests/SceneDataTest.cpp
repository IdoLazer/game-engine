#include <gtest/gtest.h>
#include "Core/SceneData.h"
#include "Entity/Entity.h"
#include "IO/FileSystem.h"
#include "Types/PropertyParsing.h"
#include "Types/TypeRegistrationMacros.h"

#include <filesystem>

using namespace Engine;

// A property type the engine knows nothing about, to prove a game can extend the parser for its
// own types - and that a block value reaches one intact.
struct TestRows
{
    std::vector<std::string> rows;
};

namespace Engine
{
    template <>
    struct PropertyParser<TestRows>
    {
        static std::optional<TestRows> Parse(std::string_view text)
        {
            if (text.empty())
                return std::nullopt;

            TestRows parsed;
            std::size_t start = 0;
            while (true)
            {
                std::size_t newline = text.find('\n', start);
                if (newline == std::string_view::npos)
                {
                    parsed.rows.emplace_back(text.substr(start));
                    return parsed;
                }

                parsed.rows.emplace_back(text.substr(start, newline - start));
                start = newline + 1;
            }
        }
    };
}

class SceneTestEntity : public Engine::Entity
{
    DECLARE_TYPE(SceneTestEntity, Entity)

    float m_speed{0.0f};
    std::string m_label;
    Vec2 m_offset{};
    TestRows m_rows;
    Engine::Entity *m_target{nullptr};

public:
    float GetSpeed() const { return m_speed; }
    const std::string &GetLabel() const { return m_label; }
    Vec2 GetOffset() const { return m_offset; }
    const TestRows &GetRows() const { return m_rows; }
};

BEGIN_TYPE_REGISTER(SceneTestEntity)
    REGISTER_PROPERTY(float, Speed, &SceneTestEntity::m_speed)
    REGISTER_PROPERTY(std::string, Label, &SceneTestEntity::m_label)
    REGISTER_PROPERTY(Vec2, Offset, &SceneTestEntity::m_offset)
    REGISTER_PROPERTY(TestRows, Rows, &SceneTestEntity::m_rows)
    REGISTER_PROPERTY(Engine::Entity *, Target, &SceneTestEntity::m_target)
END_TYPE_REGISTER()

namespace
{
    std::vector<SceneDocumentSection> Parse(std::string_view text)
    {
        return ParseSceneDocument(text, "test");
    }
}

// --- Sections ---

TEST(SceneDocumentTest, ReadsSectionsAndEntries)
{
    auto sections = Parse("[Alpha]\nOne = 1\nTwo = 2\n\n[Beta]\nThree = 3\n");

    ASSERT_EQ(sections.size(), 2u);
    EXPECT_EQ(sections[0].typeName, "Alpha");
    ASSERT_EQ(sections[0].entries.size(), 2u);
    EXPECT_EQ(sections[0].entries[0].key, "One");
    EXPECT_EQ(sections[0].entries[0].value, "1");
    EXPECT_EQ(sections[1].typeName, "Beta");
    ASSERT_EQ(sections[1].entries.size(), 1u);
    EXPECT_EQ(sections[1].entries[0].key, "Three");
}

TEST(SceneDocumentTest, RecordsLineNumbers)
{
    auto sections = Parse("// a comment\n\n[Alpha]\nOne = 1\n");

    ASSERT_EQ(sections.size(), 1u);
    EXPECT_EQ(sections[0].line, 3);
    ASSERT_EQ(sections[0].entries.size(), 1u);
    EXPECT_EQ(sections[0].entries[0].line, 4);
}

TEST(SceneDocumentTest, SkipsCommentsAndBlankLines)
{
    auto sections = Parse("// header\n\n[Alpha]\n// about One\nOne = 1\n\n");

    ASSERT_EQ(sections.size(), 1u);
    ASSERT_EQ(sections[0].entries.size(), 1u);
    EXPECT_EQ(sections[0].entries[0].key, "One");
}

TEST(SceneDocumentTest, TrimsSectionNamesKeysAndValues)
{
    auto sections = Parse("[  Alpha  ]\n   One   =   1   \n");

    ASSERT_EQ(sections.size(), 1u);
    EXPECT_EQ(sections[0].typeName, "Alpha");
    EXPECT_EQ(sections[0].entries[0].key, "One");
    EXPECT_EQ(sections[0].entries[0].value, "1");
}

TEST(SceneDocumentTest, RepeatedSectionsOfTheSameTypeEachBecomeTheirOwn)
{
    auto sections = Parse("[Pawn]\nOne = 1\n\n[Pawn]\nOne = 2\n");

    ASSERT_EQ(sections.size(), 2u);
    EXPECT_EQ(sections[0].typeName, "Pawn");
    EXPECT_EQ(sections[1].typeName, "Pawn");
    EXPECT_EQ(sections[1].entries[0].value, "2");
}

TEST(SceneDocumentTest, SplitsOnTheFirstEqualsOnly)
{
    auto sections = Parse("[Alpha]\nLabel = a = b\n");

    EXPECT_EQ(sections[0].entries[0].key, "Label");
    EXPECT_EQ(sections[0].entries[0].value, "a = b");
}

TEST(SceneDocumentTest, AnEmptyDocumentHasNoSections)
{
    EXPECT_TRUE(Parse("").empty());
    EXPECT_TRUE(Parse("// nothing but a comment\n").empty());
}

// --- Block Values ---

TEST(SceneDocumentTest, BlockValueRunsUntilABlankLine)
{
    auto sections = Parse("[Alpha]\nRows = |\nfirst\nsecond\n\nNext = 1\n");

    ASSERT_EQ(sections[0].entries.size(), 2u);
    EXPECT_EQ(sections[0].entries[0].value, "first\nsecond");
    EXPECT_EQ(sections[0].entries[1].key, "Next");
}

TEST(SceneDocumentTest, BlockValueRunsUntilTheNextSection)
{
    auto sections = Parse("[Alpha]\nRows = |\nfirst\nsecond\n[Beta]\nOne = 1\n");

    ASSERT_EQ(sections.size(), 2u);
    EXPECT_EQ(sections[0].entries[0].value, "first\nsecond");
    EXPECT_EQ(sections[1].typeName, "Beta");
}

TEST(SceneDocumentTest, BlockValueRunsUntilEndOfDocument)
{
    auto sections = Parse("[Alpha]\nRows = |\nfirst\nsecond");

    EXPECT_EQ(sections[0].entries[0].value, "first\nsecond");
}

// Leading characters are what a tile row is made of, so only trailing space is dropped.
TEST(SceneDocumentTest, BlockValueKeepsLeadingButNotTrailingWhitespace)
{
    auto sections = Parse("[Alpha]\nRows = |\n  indented  \n#..#\n");

    EXPECT_EQ(sections[0].entries[0].value, "  indented\n#..#");
}

TEST(SceneDocumentTest, CommentMarkersInsideABlockAreContent)
{
    auto sections = Parse("[Alpha]\nRows = |\n// not a comment here\n");

    EXPECT_EQ(sections[0].entries[0].value, "// not a comment here");
}

// --- Malformed Input ---

TEST(SceneDocumentTest, SkipsLinesThatAreNeitherSectionNorProperty)
{
    auto sections = Parse("[Alpha]\njust some words\nOne = 1\n");

    ASSERT_EQ(sections[0].entries.size(), 1u);
    EXPECT_EQ(sections[0].entries[0].key, "One");
}

TEST(SceneDocumentTest, SkipsPropertiesBeforeAnySection)
{
    auto sections = Parse("One = 1\n[Alpha]\nTwo = 2\n");

    ASSERT_EQ(sections.size(), 1u);
    ASSERT_EQ(sections[0].entries.size(), 1u);
    EXPECT_EQ(sections[0].entries[0].key, "Two");
}

TEST(SceneDocumentTest, SkipsSectionsAndPropertiesWithNoName)
{
    EXPECT_TRUE(Parse("[]\nOne = 1\n").empty());

    auto sections = Parse("[Alpha]\n = 1\nTwo = 2\n");
    ASSERT_EQ(sections[0].entries.size(), 1u);
    EXPECT_EQ(sections[0].entries[0].key, "Two");
}

// --- SceneData ---

class SceneDataTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        m_directory = std::filesystem::temp_directory_path() / "SceneDataTest";
        std::filesystem::remove_all(m_directory);
        std::filesystem::create_directories(m_directory);
    }

    void TearDown() override
    {
        std::filesystem::remove_all(m_directory);
    }

    // Returns the path so a test can hand it straight to SceneData.
    std::string WriteDocument(const std::string &contents)
    {
        std::filesystem::path path = m_directory / "scene.scene";
        FileSystem::WriteTextFile(path, contents);
        return path.string();
    }

    std::filesystem::path m_directory;
};

TEST_F(SceneDataTest, BuildsEntityInfoWithParsedProperties)
{
    SceneData scene(WriteDocument("[SceneTestEntity]\nSpeed = 9.5\nOffset = 1, 2\nLabel = hello\n"));

    ASSERT_FALSE(scene.IsEmpty());
    ASSERT_EQ(scene.GetEntities().size(), 1u);

    const Scene::EntityInfo &info = scene.GetEntities()[0];
    EXPECT_EQ(info.typeName, "SceneTestEntity");
    EXPECT_FLOAT_EQ(std::any_cast<float>(info.properties.at("Speed")), 9.5f);
    EXPECT_EQ(std::any_cast<Vec2>(info.properties.at("Offset")), Vec2(1.0f, 2.0f));
    EXPECT_EQ(std::any_cast<std::string>(info.properties.at("Label")), "hello");
}

TEST_F(SceneDataTest, BlockValuesReachAGameRegisteredParser)
{
    SceneData scene(WriteDocument("[SceneTestEntity]\nRows = |\n#..#\n#@.#\n"));

    ASSERT_EQ(scene.GetEntities().size(), 1u);

    const TestRows &rows = std::any_cast<const TestRows &>(scene.GetEntities()[0].properties.at("Rows"));
    ASSERT_EQ(rows.rows.size(), 2u);
    EXPECT_EQ(rows.rows[0], "#..#");
    EXPECT_EQ(rows.rows[1], "#@.#");
}

TEST_F(SceneDataTest, AppliedPropertiesEndUpOnTheEntity)
{
    SceneData scene(WriteDocument("[SceneTestEntity]\nSpeed = 4\nOffset = 3, 5\n"));

    SceneTestEntity entity;
    const Scene::EntityInfo &info = scene.GetEntities()[0];
    TypeRegistry::Get().SetProperties(&entity, info.typeName, info.properties);

    EXPECT_FLOAT_EQ(entity.GetSpeed(), 4.0f);
    EXPECT_EQ(entity.GetOffset(), Vec2(3.0f, 5.0f));
}

TEST_F(SceneDataTest, SkipsUnknownTypes)
{
    SceneData scene(WriteDocument("[NotARegisteredType]\nSpeed = 1\n\n[SceneTestEntity]\nSpeed = 2\n"));

    ASSERT_EQ(scene.GetEntities().size(), 1u);
    EXPECT_EQ(scene.GetEntities()[0].typeName, "SceneTestEntity");
}

TEST_F(SceneDataTest, SkipsUnknownPropertiesButKeepsTheRest)
{
    SceneData scene(WriteDocument("[SceneTestEntity]\nWobble = 1\nSpeed = 2\n"));

    const Scene::EntityInfo &info = scene.GetEntities()[0];
    EXPECT_EQ(info.properties.count("Wobble"), 0u);
    EXPECT_FLOAT_EQ(std::any_cast<float>(info.properties.at("Speed")), 2.0f);
}

TEST_F(SceneDataTest, SkipsValuesThatDoNotParseButKeepsTheRest)
{
    SceneData scene(WriteDocument("[SceneTestEntity]\nSpeed = quickly\nOffset = 1, 2\n"));

    const Scene::EntityInfo &info = scene.GetEntities()[0];
    EXPECT_EQ(info.properties.count("Speed"), 0u);
    EXPECT_EQ(info.properties.count("Offset"), 1u);
}

TEST_F(SceneDataTest, SkipsPropertiesWithNoTextForm)
{
    SceneData scene(WriteDocument("[SceneTestEntity]\nTarget = something\nSpeed = 1\n"));

    const Scene::EntityInfo &info = scene.GetEntities()[0];
    EXPECT_EQ(info.properties.count("Target"), 0u);
    EXPECT_EQ(info.properties.count("Speed"), 1u);
}

TEST_F(SceneDataTest, AMissingFileLeavesTheSceneEmpty)
{
    SceneData scene((m_directory / "no_such_file.scene").string());

    EXPECT_TRUE(scene.IsEmpty());
    EXPECT_FALSE(scene.Reload());
}

TEST_F(SceneDataTest, ReloadPicksUpAChangedFile)
{
    std::string path = WriteDocument("[SceneTestEntity]\nSpeed = 1\n");
    SceneData scene(path);
    ASSERT_FLOAT_EQ(std::any_cast<float>(scene.GetEntities()[0].properties.at("Speed")), 1.0f);

    FileSystem::WriteTextFile(path, "[SceneTestEntity]\nSpeed = 7\n");
    ASSERT_TRUE(scene.Reload());

    EXPECT_FLOAT_EQ(std::any_cast<float>(scene.GetEntities()[0].properties.at("Speed")), 7.0f);
}

TEST_F(SceneDataTest, AFailedReloadKeepsTheEntitiesItAlreadyHad)
{
    std::string path = WriteDocument("[SceneTestEntity]\nSpeed = 1\n");
    SceneData scene(path);

    FileSystem::WriteTextFile(path, "// every section is broken now\n[]\n");
    EXPECT_FALSE(scene.Reload());

    ASSERT_EQ(scene.GetEntities().size(), 1u);
    EXPECT_FLOAT_EQ(std::any_cast<float>(scene.GetEntities()[0].properties.at("Speed")), 1.0f);
}
