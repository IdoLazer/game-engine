#pragma once

#include <memory>
#include <vector>
#include <algorithm>
#include <type_traits>
#include "../Entity/Entity.h"

namespace Engine
{
    class Scene
    {
    public:
        Scene() = default;
        ~Scene() = default;

        // Creates an entity, assigns its scene, initializes it, and stores it.
        // Returns a non-owning raw pointer for the caller to use.
        template <typename T, typename... Args>
        T *Instantiate(Args &&...args)
        {
            static_assert(std::is_base_of_v<Entity, T>, "Instantiate requires a type derived from Entity");
            auto entity = std::make_unique<T>(std::forward<Args>(args)...);
            entity->SetScene(this);
            entity->Initialize();
            T *raw = entity.get();
            m_entities.push_back(std::move(entity));
            return raw;
        }

        // Removes and destroys an entity.
        void Destroy(Entity *entity)
        {
            auto it = std::find_if(m_entities.begin(), m_entities.end(),
                [entity](const std::unique_ptr<Entity> &e) { return e.get() == entity; });
            if (it != m_entities.end())
                m_entities.erase(it);
        }

        // Destroys all entities.
        void Clear() { m_entities.clear(); }

    private:
        std::vector<std::unique_ptr<Entity>> m_entities;
    };
}
