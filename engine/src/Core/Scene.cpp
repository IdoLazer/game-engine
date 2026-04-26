#include "Scene.h"
#include <algorithm>

namespace Engine
{

    // --- Constructors & Destructors ---

    Scene::~Scene() { Clear(); }

    // --- Instantiation ---

    Entity *Scene::Instantiate(const std::string &typeName)
    {
        Entity *entity = TypeRegistry::Get().Create(typeName);
        if (!entity)
            return nullptr;

        std::unique_ptr<Entity> owned(entity);
        owned->SetScene(this);
        Entity *raw = owned.get();
        m_pendingEntities.push_back(std::move(owned));
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

    void Scene::Update(float deltaTime)
    {
        FlushPending();

        for (auto &entity : m_entities)
        {
            if (!entity->m_pendingDestroy)
                entity->Update(deltaTime);
        }

        FlushDestroyed();
    }

    void Scene::Render() const
    {
        for (auto &entity : m_entities)
        {
            if (!entity->m_pendingDestroy)
                entity->Render();
        }
    }

    void Scene::Destroy(Entity *entity)
    {
        entity->m_pendingDestroy = true;
    }

    void Scene::Clear()
    {
        for (auto &entity : m_entities)
            entity->Destroy();
        m_entities.clear();

        for (auto &entity : m_pendingEntities)
            entity->Destroy();
        m_pendingEntities.clear();
    }

    // --- Internal ---

    void Scene::FlushPending()
    {
        // Move pending entities into the main list and initialize them.
        // We drain by index because Initialize() could instantiate more entities.
        while (!m_pendingEntities.empty())
        {
            // Take a snapshot of current pending count
            auto batch = std::move(m_pendingEntities);
            m_pendingEntities.clear();

            for (auto &entity : batch)
            {
                entity->Initialize();
                entity->m_initialized = true;
                m_entities.push_back(std::move(entity));
            }
            // If Initialize() spawned new entities, they're now in m_pendingEntities
            // and we loop again to flush those too.
        }
    }

    void Scene::FlushDestroyed()
    {
        // Call Destroy() on each entity marked for removal
        for (auto &entity : m_entities)
        {
            if (entity->m_pendingDestroy)
                entity->Destroy();
        }

        // Now erase them
        auto it = std::remove_if(m_entities.begin(), m_entities.end(),
            [](const std::unique_ptr<Entity> &e) { return e->m_pendingDestroy; });
        m_entities.erase(it, m_entities.end());
    }

}
