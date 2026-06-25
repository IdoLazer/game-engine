#pragma once

#include <memory>
#include <vector>
#include <concepts>
#include <string>
#include <unordered_map>
#include <any>
#include "../Entity/Entity.h"
#include "../Types/TypeRegistry.h"

namespace Engine
{
    class Scene
    {
    // --- Types ---
    public:
        struct EntityInfo
        {
            std::string typeName;
            PropertyMap properties;
        };

    // --- Fields ---
    private:
        std::vector<std::unique_ptr<Entity>> m_entities;
        std::vector<std::unique_ptr<Entity>> m_pendingEntities;

    // --- Constructors & Destructors ---
    public:
        Scene() = default;
        ~Scene();

    // --- Instantiation ---
    public:
        // Template-based — creates a default-constructed entity.
        template <typename T>
            requires std::derived_from<T, Entity>
        T *Instantiate()
        {
            auto entity = std::make_unique<T>();
            entity->SetScene(this);
            T *raw = entity.get();
            m_pendingEntities.push_back(std::move(entity));
            return raw;
        }

        // String-based — creates an entity by registered type name.
        Entity *Instantiate(const std::string &typeName);

        // Data-driven — creates by type name and sets properties from a map (e.g. from a scene file).
        Entity *Instantiate(const EntityInfo &info);

    // --- Entity Retrieval ---
    public:
        template <typename T>
            requires std::derived_from<T, Entity>
        std::vector<T *> GetAllEntitiesOfType() const
        {
            std::vector<T *> result;
            for (const auto &entity : m_entities)
            {
                if (auto *casted = dynamic_cast<T *>(entity.get()))
                    result.push_back(casted);
            }
            for (const auto &entity : m_pendingEntities)
            {
                if (auto *casted = dynamic_cast<T *>(entity.get()))
                    result.push_back(casted);
            }
            return result;
        }

        template <typename T>
            requires std::derived_from<T, Entity>
        T *GetFirstEntityOfType() const
        {
            for (const auto &entity : m_entities)
            {
                if (auto *casted = dynamic_cast<T *>(entity.get()))
                    return casted;
            }
            for (const auto &entity : m_pendingEntities)
            {
                if (auto *casted = dynamic_cast<T *>(entity.get()))
                    return casted;
            }
            return nullptr;
        }

    // --- Lifecycle ---
    public:
        void Update(float deltaTime);
        void Render() const;
        void Destroy(Entity *entity);
        void Clear();

    // --- Internal ---
    private:
        void FlushPending();
        void FlushDestroyed();
    };
}
