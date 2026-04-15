#pragma once

#include "../Math/Vec2.h"
#include "../Graphics/Color.h"
#include "../Types/TypeRegistrationMacros.h"

namespace Engine
{
    class Scene;

    // Base class for all game world entities.
    // Represents something in the game world with position, size, and behavior.
    class Entity
    {
        friend class Scene;
        DECLARE_TYPE_NO_BASE(Entity)

    // --- Fields ---
    protected:
        Vec2 m_worldPosition{};
        Vec2 m_worldSize{};
        Color m_color{};

    // --- Constructors & Destructors ---
    public:
        Entity() = default;
        virtual ~Entity() = default;

    // --- Lifecycle ---
    public:
        virtual void Initialize();
        virtual void Update(float deltaTime);
        virtual void Render() const;

    // --- Accessors ---
    public:
        Vec2 GetWorldPosition() const;
        Vec2 GetWorldSize() const;
        Color GetColor() const;
        void SetWorldPosition(const Vec2 &position);
        void SetWorldSize(const Vec2 &size);
        void SetColor(const Color &color);

    // --- Internal ---
    protected:
        Scene *GetScene() const;

    private:
        void SetScene(Scene *scene);
        Scene *m_scene{nullptr};
    };
}