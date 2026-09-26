#include <gtest/gtest.h>
#include "IO/FileSystem.h"

#include <filesystem>

using namespace Engine;

// Each test gets its own directory under the system temp dir, so runs can't collide.
class FileSystemTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        m_directory = std::filesystem::temp_directory_path() /
                      ("FileSystemTest_" + std::string(::testing::UnitTest::GetInstance()->current_test_info()->name()));
        std::filesystem::remove_all(m_directory);
        std::filesystem::create_directories(m_directory);
    }

    void TearDown() override
    {
        std::filesystem::remove_all(m_directory);
    }

    std::filesystem::path PathTo(const std::string &fileName) const
    {
        return m_directory / fileName;
    }

    std::filesystem::path m_directory;
};

// --- Round Trip ---

TEST_F(FileSystemTest, ReadsBackWhatItWrote)
{
    const std::string contents = "line one\nline two\n";
    ASSERT_TRUE(FileSystem::WriteTextFile(PathTo("round_trip.txt"), contents));

    auto readBack = FileSystem::ReadTextFile(PathTo("round_trip.txt"));
    ASSERT_TRUE(readBack.has_value());
    EXPECT_EQ(*readBack, contents);
}

TEST_F(FileSystemTest, WriteOverwritesExistingFile)
{
    ASSERT_TRUE(FileSystem::WriteTextFile(PathTo("overwrite.txt"), "original contents"));
    ASSERT_TRUE(FileSystem::WriteTextFile(PathTo("overwrite.txt"), "short"));

    auto readBack = FileSystem::ReadTextFile(PathTo("overwrite.txt"));
    ASSERT_TRUE(readBack.has_value());
    EXPECT_EQ(*readBack, "short");
}

TEST_F(FileSystemTest, WritesAndReadsEmptyContent)
{
    ASSERT_TRUE(FileSystem::WriteTextFile(PathTo("empty.txt"), ""));

    auto readBack = FileSystem::ReadTextFile(PathTo("empty.txt"));
    ASSERT_TRUE(readBack.has_value());
    EXPECT_EQ(*readBack, "");
}

// --- Failure Cases ---

TEST_F(FileSystemTest, ReadingMissingFileReturnsNullopt)
{
    EXPECT_FALSE(FileSystem::ReadTextFile(PathTo("does_not_exist.txt")).has_value());
}

TEST_F(FileSystemTest, ReadingADirectoryReturnsNullopt)
{
    EXPECT_FALSE(FileSystem::ReadTextFile(m_directory).has_value());
}

TEST_F(FileSystemTest, WritingIntoAMissingDirectoryFails)
{
    EXPECT_FALSE(FileSystem::WriteTextFile(PathTo("no_such_folder/file.txt"), "content"));
}

// --- Locations ---

TEST_F(FileSystemTest, ExecutableDirectoryIsTheDirectoryHoldingTheTestExecutable)
{
    EXPECT_TRUE(std::filesystem::is_directory(FileSystem::GetExecutableDirectory()));
}
