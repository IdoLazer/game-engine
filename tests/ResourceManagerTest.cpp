#include <gtest/gtest.h>
#include "Resources/ResourceManager.h"

using namespace Engine;

namespace
{
    // Stands in for a real resource: records how often it was built and reloaded, without
    // touching the disk or the GPU.
    class FakeResource : public Resource
    {
    public:
        explicit FakeResource(const std::string &path)
        {
            m_path = path;
            ++s_constructions;
        }

        bool Reload() override
        {
            ++m_reloadCount;
            return m_reloadSucceeds;
        }

        int GetReloadCount() const { return m_reloadCount; }
        void SetReloadSucceeds(bool succeeds) { m_reloadSucceeds = succeeds; }

        static int s_constructions;

    private:
        int m_reloadCount = 0;
        bool m_reloadSucceeds = true;
    };

    int FakeResource::s_constructions = 0;

    // Deliberately does not override Reload(), so it also covers the base class default.
    class UnreloadableResource : public Resource
    {
    public:
        explicit UnreloadableResource(const std::string &path) { m_path = path; }
    };
}

class ResourceManagerTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        FakeResource::s_constructions = 0;
        ResourceManager::Initialize();
    }

    void TearDown() override
    {
        ResourceManager::Shutdown();
    }
};

// --- Caching ---

TEST_F(ResourceManagerTest, RepeatedLoadsShareOneInstance)
{
    auto *first = ResourceManager::Load<FakeResource>("fake.res");
    auto *second = ResourceManager::Load<FakeResource>("fake.res");

    ASSERT_NE(first, nullptr);
    EXPECT_EQ(first, second);
    EXPECT_EQ(FakeResource::s_constructions, 1);
}

TEST_F(ResourceManagerTest, DifferentPathsGetDifferentInstances)
{
    auto *first = ResourceManager::Load<FakeResource>("one.res");
    auto *second = ResourceManager::Load<FakeResource>("two.res");

    EXPECT_NE(first, second);
    EXPECT_EQ(FakeResource::s_constructions, 2);
}

TEST_F(ResourceManagerTest, ShutdownReleasesCachedResources)
{
    ResourceManager::Load<FakeResource>("fake.res");
    ResourceManager::Shutdown();
    ResourceManager::Initialize();

    ResourceManager::Load<FakeResource>("fake.res");
    EXPECT_EQ(FakeResource::s_constructions, 2);
}

// --- Cache Modes ---

TEST_F(ResourceManagerTest, ReuseIsTheDefaultAndDoesNotReload)
{
    ResourceManager::Load<FakeResource>("fake.res");
    auto *reused = ResourceManager::Load<FakeResource>("fake.res");

    EXPECT_EQ(reused->GetReloadCount(), 0);
}

TEST_F(ResourceManagerTest, RefreshReloadsTheCachedInstanceInPlace)
{
    auto *original = ResourceManager::Load<FakeResource>("fake.res");
    auto *refreshed = ResourceManager::Load<FakeResource>("fake.res", CacheMode::Refresh);

    EXPECT_EQ(original, refreshed);
    EXPECT_EQ(FakeResource::s_constructions, 1);
    EXPECT_EQ(refreshed->GetReloadCount(), 1);
}

TEST_F(ResourceManagerTest, RefreshOnAnUncachedPathJustLoadsIt)
{
    auto *loaded = ResourceManager::Load<FakeResource>("fake.res", CacheMode::Refresh);

    ASSERT_NE(loaded, nullptr);
    EXPECT_EQ(FakeResource::s_constructions, 1);
    EXPECT_EQ(loaded->GetReloadCount(), 0);
}

TEST_F(ResourceManagerTest, AFailedRefreshKeepsTheCachedResource)
{
    auto *original = ResourceManager::Load<FakeResource>("fake.res");
    original->SetReloadSucceeds(false);

    auto *refreshed = ResourceManager::Load<FakeResource>("fake.res", CacheMode::Refresh);

    EXPECT_EQ(original, refreshed);
    EXPECT_EQ(refreshed->GetReloadCount(), 1);
}

TEST_F(ResourceManagerTest, RefreshingAResourceThatCannotReloadKeepsIt)
{
    auto *original = ResourceManager::Load<UnreloadableResource>("plain.res");
    auto *refreshed = ResourceManager::Load<UnreloadableResource>("plain.res", CacheMode::Refresh);

    EXPECT_EQ(original, refreshed);
}

// --- Type Safety ---

TEST_F(ResourceManagerTest, LoadingACachedPathAsTheWrongTypeReturnsNullptr)
{
    ResourceManager::Load<FakeResource>("fake.res");

    EXPECT_EQ(ResourceManager::Load<UnreloadableResource>("fake.res"), nullptr);
}

// --- Resource Defaults ---

TEST_F(ResourceManagerTest, ResourcesDoNotReloadUnlessTheyImplementIt)
{
    UnreloadableResource plain("plain.res");
    EXPECT_FALSE(plain.Reload());
}
