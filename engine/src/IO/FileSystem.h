#pragma once

#include <filesystem>
#include <optional>
#include <string>

namespace Engine
{
    class FileSystem
    {
    // --- File Access ---
    public:
        // Text mode: reads normalize CRLF to LF, writes use the platform's own line endings.
        static std::optional<std::string> ReadTextFile(const std::filesystem::path &path);
        static bool WriteTextFile(const std::filesystem::path &path, const std::string &contents);

    // --- Locations ---
    public:
        static const std::filesystem::path &GetExecutableDirectory();
    };
}
