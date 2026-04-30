#include "ResourceManager.h"
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#endif

namespace Engine
{
    std::unordered_map<std::string, std::unique_ptr<Resource>> ResourceManager::s_resources;
    std::filesystem::path ResourceManager::s_basePath;

    void ResourceManager::Initialize()
    {
        s_resources.clear();

        // Resolve the directory containing the running executable.
        // All asset paths are relative to this directory.
#ifdef _WIN32
        char buffer[MAX_PATH];
        GetModuleFileNameA(nullptr, buffer, MAX_PATH);
        s_basePath = std::filesystem::path(buffer).parent_path();
#else
        // On other platforms, fall back to cwd.
        s_basePath = std::filesystem::current_path();
#endif

        std::cout << "ResourceManager initialized. Base path: " << s_basePath.string() << std::endl;
    }

    void ResourceManager::Shutdown()
    {
        s_resources.clear();
        s_basePath.clear();
        std::cout << "ResourceManager shut down. All resources released." << std::endl;
    }
}
