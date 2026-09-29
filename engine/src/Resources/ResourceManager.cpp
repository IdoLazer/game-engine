#include "ResourceManager.h"
#include "../IO/FileSystem.h"
#include <iostream>

namespace Engine
{
    std::unordered_map<std::string, std::unique_ptr<Resource>> ResourceManager::s_resources;
    std::filesystem::path ResourceManager::s_assetRoot;

    void ResourceManager::SetAssetRoot(const std::filesystem::path &root)
    {
        s_assetRoot = root;
    }

    void ResourceManager::Initialize()
    {
        s_resources.clear();

        // Assets ship next to the executable, so that is where they are read from unless something
        // pointed the root elsewhere first.
        if (s_assetRoot.empty())
            s_assetRoot = FileSystem::GetExecutableDirectory();

        std::cout << "ResourceManager initialized. Asset root: " << s_assetRoot.string() << std::endl;
    }

    void ResourceManager::Shutdown()
    {
        s_resources.clear();
        s_assetRoot.clear();
        std::cout << "ResourceManager shut down. All resources released." << std::endl;
    }
}
