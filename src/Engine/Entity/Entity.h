#pragma once

#include "../Math/Vec2.h"
#include "../Graphics/Color.h"

namespace Engine
{
    class Scene; // Forward declaration — Scene sets itself on entities it creates

    // Base class for all game world entities
    // Represents something in the game world with position, size, and behavior
    class Entity
    {
        friend class Scene; // Scene needs to call SetScene()

    protected:
        Vec2 m_worldPosition; // Position in world coordinates
        Vec2 m_worldSize;     // Size in world units
        Color m_color;        // Visual color for rendering

        Scene *GetScene() const { return m_scene; }

    public:
        Entity() = default;

        Entity(const Vec2 &position, const Vec2 &size, const Color &color)
            : m_worldPosition(position), m_worldSize(size), m_color(color) {}

        virtual ~Entity() = default;

        // Core entity lifecycle - can be overridden by subclasses, default to empty implementation
        virtual void Initialize() {}
        virtual void Update(float deltaTime) {}
        virtual void Render() const {}

        // World property accessors
        Vec2 GetWorldPosition() const { return m_worldPosition; }
        Vec2 GetWorldSize() const { return m_worldSize; }
        Color GetColor() const { return m_color; }

        void SetWorldPosition(const Vec2 &position) { m_worldPosition = position; }
        void SetWorldSize(const Vec2 &size) { m_worldSize = size; }
        void SetColor(const Color &color) { m_color = color; }

    private:
        void SetScene(Scene *scene) { m_scene = scene; }
        Scene *m_scene{nullptr};
    };
}