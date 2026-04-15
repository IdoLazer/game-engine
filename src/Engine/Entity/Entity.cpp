#include "Entity.h"

namespace Engine
{

    // --- Type Registration ---

    BEGIN_TYPE_REGISTER(Entity)
        REGISTER_PROPERTY(Vec2, WorldPosition, &Entity::m_worldPosition)
        REGISTER_PROPERTY(Vec2, WorldSize, &Entity::m_worldSize)
        REGISTER_PROPERTY(Color, Color, &Entity::m_color)
    END_TYPE_REGISTER()

    // --- Lifecycle ---

    void Entity::Initialize() {}
    void Entity::Update(float deltaTime) {}
    void Entity::Render() const {}

    // --- Accessors ---

    Vec2 Entity::GetWorldPosition() const { return m_worldPosition; }
    Vec2 Entity::GetWorldSize() const { return m_worldSize; }
    Color Entity::GetColor() const { return m_color; }
    void Entity::SetWorldPosition(const Vec2 &position) { m_worldPosition = position; }
    void Entity::SetWorldSize(const Vec2 &size) { m_worldSize = size; }
    void Entity::SetColor(const Color &color) { m_color = color; }

    // --- Internal ---

    Scene *Entity::GetScene() const { return m_scene; }
    void Entity::SetScene(Scene *scene) { m_scene = scene; }

}
