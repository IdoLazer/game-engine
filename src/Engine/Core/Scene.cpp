#include "Scene.h"
#include <algorithm>

namespace Engine
{

    // --- Instantiation ---

    Entity *Scene::Instantiate(const std::string &typeName)
    {
        Entity *entity = TypeRegistry::Get().Create(typeName);
        if (!entity)
            return nullptr;

        std::unique_ptr<Entity> owned(entity);
        owned->SetScene(this);
        Entity *raw = owned.get();
        m_entities.push_back(std::move(owned));
        return raw;
    }

    Entity *Scene::Instantiate(const EntityInfo &info)
    {
        Entity *entity = Instantiate(info.typeName);
        if (!entity)
            return nullptr;

        TypeRegistry::Get().SetProperties(entity, info.typeName, info.properties);
        return entity;
    }

    // --- Lifecycle ---

    void Scene::Destroy(Entity *entity)
    {
        auto it = std::find_if(m_entities.begin(), m_entities.end(),
            [entity](const std::unique_ptr<Entity> &e) { return e.get() == entity; });
        if (it != m_entities.end())
            m_entities.erase(it);
    }

    void Scene::Clear() { m_entities.clear(); }

}
