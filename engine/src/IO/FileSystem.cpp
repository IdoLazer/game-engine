#include "FileSystem.h"

#include <fstream>
#include <sstream>

#ifdef _WIN32
#include <windows.h>
#elif __APPLE__
#include <mach-o/dyld.h>
#endif

namespace Engine
{
    namespace
    {
        std::filesystem::path FindExecutableDirectory()
        {
#ifdef _WIN32
            char buffer[MAX_PATH];
            GetModuleFileNameA(nullptr, buffer, MAX_PATH);
            return std::filesystem::path(buffer).parent_path();
#elif __APPLE__
            char buffer[PATH_MAX];
            uint32_t bufferSize = sizeof(buffer);
            _NSGetExecutablePath(buffer, &bufferSize);
            return std::filesystem::path(buffer).parent_path();
#else
            return std::filesystem::current_path();
#endif
        }
    }

    std::optional<std::string> FileSystem::ReadTextFile(const std::filesystem::path &path)
    {
        std::ifstream file(path);
        if (!file)
            return std::nullopt;

        std::ostringstream contents;
        contents << file.rdbuf();
        return contents.str();
    }

    bool FileSystem::WriteTextFile(const std::filesystem::path &path, const std::string &contents)
    {
        std::ofstream file(path);
        if (!file)
            return false;

        file << contents;
        return file.good();
    }

    const std::filesystem::path &FileSystem::GetExecutableDirectory()
    {
        static const std::filesystem::path directory = FindExecutableDirectory();
        return directory;
    }
}
