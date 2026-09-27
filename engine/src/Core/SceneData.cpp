#include "SceneData.h"
#include "../IO/FileSystem.h"
#include "../Types/TypeRegistry.h"
#include "../Utilities/StringUtils.h"

#include <iostream>
#include <sstream>

namespace Engine
{
    namespace
    {
        constexpr std::string_view BLOCK_VALUE = "|";

        // Matches a compiler's "file:line: message", so an editor can jump straight to it.
        std::ostream &Problem(std::string_view sourceName, int line)
        {
            return std::cerr << sourceName << ":" << line << ": ";
        }
    }

    std::vector<SceneDocumentSection> ParseSceneDocument(std::string_view text, std::string_view sourceName)
    {
        std::vector<SceneDocumentSection> sections;
        std::istringstream stream{std::string(text)};
        std::string line;
        int lineNumber = 0;

        bool insideBlock = false;
        std::string blockValue;

        auto closeBlock = [&sections, &insideBlock, &blockValue]()
        {
            sections.back().entries.back().value = blockValue;
            insideBlock = false;
            blockValue.clear();
        };

        while (std::getline(stream, line))
        {
            ++lineNumber;

            if (line.ends_with('\r'))
                line.pop_back();

            if (insideBlock)
            {
                bool stillBlockContent = !Trim(line).empty() && !line.starts_with('[');
                if (stillBlockContent)
                {
                    if (!blockValue.empty())
                        blockValue += '\n';
                    blockValue += TrimEnd(line);
                    continue;
                }
                closeBlock();
            }

            std::string_view trimmed = Trim(line);
            if (trimmed.empty() || trimmed.starts_with("//"))
                continue;

            if (trimmed.starts_with('[') && trimmed.ends_with(']'))
            {
                std::string_view typeName = Trim(trimmed.substr(1, trimmed.size() - 2));
                if (typeName.empty())
                {
                    Problem(sourceName, lineNumber) << "section has no type name" << std::endl;
                    continue;
                }

                sections.push_back(SceneDocumentSection{std::string(typeName), lineNumber, {}});
                continue;
            }

            std::size_t equals = trimmed.find('=');
            if (equals == std::string_view::npos)
            {
                Problem(sourceName, lineNumber) << "expected '[Type]' or 'Key = Value'" << std::endl;
                continue;
            }

            if (sections.empty())
            {
                Problem(sourceName, lineNumber) << "property before any [Type] section" << std::endl;
                continue;
            }

            std::string_view key = Trim(trimmed.substr(0, equals));
            if (key.empty())
            {
                Problem(sourceName, lineNumber) << "property has no name" << std::endl;
                continue;
            }

            std::string_view value = Trim(trimmed.substr(equals + 1));
            if (value == BLOCK_VALUE)
            {
                insideBlock = true;
                value = {};
            }

            sections.back().entries.push_back(
                SceneDocumentEntry{std::string(key), std::string(value), lineNumber});
        }

        if (insideBlock)
            closeBlock();

        return sections;
    }

    SceneData::SceneData(const std::string &filePath)
    {
        m_path = filePath;
        LoadFromFile();
    }

    bool SceneData::Reload()
    {
        return LoadFromFile();
    }

    bool SceneData::LoadFromFile()
    {
        std::optional<std::string> text = FileSystem::ReadTextFile(m_path);
        if (!text)
        {
            std::cerr << "SceneData: could not read " << m_path << std::endl;
            return false;
        }

        const TypeRegistry &registry = TypeRegistry::Get();
        std::vector<Scene::EntityInfo> entities;

        for (const SceneDocumentSection &section : ParseSceneDocument(*text, m_path))
        {
            if (!registry.IsTypeRegistered(section.typeName))
            {
                Problem(m_path, section.line) << "unknown type '" << section.typeName << "'" << std::endl;
                continue;
            }

            Scene::EntityInfo info;
            info.typeName = section.typeName;

            for (const SceneDocumentEntry &entry : section.entries)
            {
                if (!registry.HasProperty(section.typeName, entry.key))
                {
                    Problem(m_path, entry.line) << section.typeName << " has no property '"
                                                << entry.key << "'" << std::endl;
                    continue;
                }

                std::any value = registry.ParseProperty(section.typeName, entry.key, entry.value);
                if (!value.has_value())
                {
                    Problem(m_path, entry.line) << "cannot read '" << entry.value << "' as "
                                                << section.typeName << "::" << entry.key << std::endl;
                    continue;
                }

                info.properties[entry.key] = std::move(value);
            }

            entities.push_back(std::move(info));
        }

        if (entities.empty())
        {
            std::cerr << "SceneData: no entities in " << m_path << std::endl;
            return false;
        }

        m_entities = std::move(entities);
        return true;
    }
}
