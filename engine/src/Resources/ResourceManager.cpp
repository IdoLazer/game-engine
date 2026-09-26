#include "ResourceManager.h"
#include "../IO/FileSystem.h"
#include <iostream>

namespace Engine
{
    std::unordered_map<std::string, std::unique_ptr<Resource>> ResourceManager::s_resources;
    std::filesystem::path ResourceManager::s_basePath;

    void ResourceManager::Initialize()
    {
        s_resources.clear();

        // Assets ship next to the executable, so that's what asset paths are relative to.
        s_basePath = FileSystem::GetExecutableDirectory();

        std::cout << "ResourceManager initialized. Base path: " << s_basePath.string() << std::endl;
    }

    void ResourceManager::Shutdown()
    {
        s_resources.clear();
        s_basePath.clear();
        std::cout << "ResourceManager shut down. All resources released." << std::endl;
    }
}
