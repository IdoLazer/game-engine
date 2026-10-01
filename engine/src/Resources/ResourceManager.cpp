#include "ResourceManager.h"
#include "../IO/FileSystem.h"
#include <iostream>

namespace Engine
{
    std::unordered_map<std::string, std::unique_ptr<Resource>> ResourceManager::s_resources;
    std::filesystem::path ResourceManager::s_assetRoot;

    void ResourceManager::Initialize()
    {
        s_resources.clear();

        // Debug reads assets from the game's source tree, so editing one needs no rebuild.
#ifdef GAME_SOURCE_DIR
        s_assetRoot = GAME_SOURCE_DIR;
#else
        s_assetRoot = FileSystem::GetExecutableDirectory();
#endif

        std::cout << "ResourceManager initialized. Asset root: " << s_assetRoot.string() << std::endl;
    }

    void ResourceManager::Shutdown()
    {
        s_resources.clear();
        s_assetRoot.clear();
        std::cout << "ResourceManager shut down. All resources released." << std::endl;
    }
}
