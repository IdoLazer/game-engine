#pragma once

#include <Engine.h>
#include "Grid.h"

class Food
{
private:
    Grid *m_grid;
    Vec2 m_position;
    Vec2 m_size;
    Color m_color;

public:
    Food() = default;

    void Initialize(
        Grid *grid,
        const Vec2 &startPos,
        const Vec2 &size,
        const Color &color);

    void Render() const;

    // Getters
    Vec2 GetPosition() const { return m_position; }
    Vec2 GetSize() const { return m_size; }
    Color GetColor() const { return m_color; }

    // Setters
    void SetPosition(const Vec2 &pos) { m_position = pos; }
};