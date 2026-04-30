#pragma once

#include <string>

namespace Engine
{
    // Base class for all engine-managed assets (textures, fonts, sounds, etc.).
    // The ResourceManager owns Resources and caches them by path.
    class Resource
    {
    public:
        virtual ~Resource() = default;

        const std::string& GetPath() const { return m_path; }

    protected:
        std::string m_path;
    };
}
