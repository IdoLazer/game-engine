#pragma once

#include <memory>
#include <vector>
#include <type_traits>
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

    // --- Constructors & Destructors ---
    public:
        Scene() = default;
        ~Scene() = default;

    // --- Instantiation ---
    public:
        // Template-based — creates a default-constructed entity.
        template <typename T>
        T *Instantiate()
        {
            static_assert(std::is_base_of_v<Entity, T>, "Instantiate requires a type derived from Entity");
            auto entity = std::make_unique<T>();
            entity->SetScene(this);
            T *raw = entity.get();
            m_entities.push_back(std::move(entity));
            return raw;
        }

        // String-based — creates an entity by registered type name.
        Entity *Instantiate(const std::string &typeName);

        // Data-driven — creates by type name and sets properties from a map (e.g. from a scene file).
        Entity *Instantiate(const EntityInfo &info);

    // --- Lifecycle ---
    public:
        void Destroy(Entity *entity);
        void Clear();
    };
}
