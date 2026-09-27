#pragma once

#include <string>
#include <string_view>
#include <vector>

#include "Scene.h"
#include "../Resources/Resource.h"

namespace Engine
{
    // One "Key = Value" line of a scene document.
    struct SceneDocumentEntry
    {
        std::string key;
        std::string value;
        int line{0};
    };

    // One "[TypeName]" block and the entries beneath it.
    struct SceneDocumentSection
    {
        std::string typeName;
        int line{0};
        std::vector<SceneDocumentEntry> entries;
    };

    // Splits a scene document into its sections. Blank lines and lines starting with "//" are
    // skipped. A value of "|" starts a block value, which runs until a blank line, a line starting
    // with "[", or the end of the document. `sourceName` appears only in messages.
    std::vector<SceneDocumentSection> ParseSceneDocument(std::string_view text, std::string_view sourceName);

    // The entities a scene document describes, ready to hand to Scene::Instantiate.
    class SceneData : public Resource
    {
    // --- Constructors & Destructors ---
    public:
        explicit SceneData(const std::string &filePath);

    // --- Resource Interface ---
    public:
        bool Reload() override;

    // --- Accessors ---
    public:
        const std::vector<Scene::EntityInfo> &GetEntities() const { return m_entities; }
        bool IsEmpty() const { return m_entities.empty(); }

    // --- Internal ---
    private:
        bool LoadFromFile();

    // --- Fields ---
    private:
        std::vector<Scene::EntityInfo> m_entities;
    };
}
