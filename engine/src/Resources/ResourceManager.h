#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <iostream>
#include <filesystem>
#include "Resource.h"

namespace Engine
{
    // Static subsystem that owns and caches all engine resources.
    // Access from anywhere: ResourceManager::Load<Texture2D>("assets/foo.png")
    //
    // Application manages the lifecycle (Initialize / Shutdown).
    class ResourceManager
    {
    public:
        static void Initialize();
        static void Shutdown();

        // Load a resource by path. Returns cached instance on subsequent calls.
        // T must inherit from Resource and have a constructor taking std::string.
        // Paths are resolved relative to the executable's directory.
        template<typename T>
        static T* Load(const std::string& path)
        {
            static_assert(std::derived_from<T, Resource>, "T must inherit from Resource");
            // Check cache first
            auto it = s_resources.find(path);
            if (it != s_resources.end())
            {
                // Verify the cached resource is actually the requested type.
                T* typed = dynamic_cast<T*>(it->second.get());
                if (typed)
                    return typed;

                std::cerr << "ResourceManager: Type mismatch for cached resource: "
                          << path << std::endl;
                return nullptr;
            }

            // Resolve the path relative to the executable's directory
            std::string fullPath = (s_basePath / path).string();

            // Not cached — create, cache, and return
            auto resource = std::make_unique<T>(fullPath);
            T* raw = resource.get();
            s_resources[path] = std::move(resource);

            std::cout << "ResourceManager: Loaded " << path << std::endl;
            return raw;
        }

    private:
        static std::unordered_map<std::string, std::unique_ptr<Resource>> s_resources;
        static std::filesystem::path s_basePath;  // Directory containing the executable
    };
}
