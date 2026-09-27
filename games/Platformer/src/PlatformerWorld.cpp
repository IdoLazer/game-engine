#include "PlatformerWorld.h"
#include <algorithm>
#include <cmath>
#include <limits>

namespace
{
    // Float precision slack for TouchesSolid - not a tunable detection distance.
    constexpr float kContactEpsilon = 1e-4f;
}

BEGIN_TYPE_REGISTER(PlatformerWorld)
    REGISTER_PROPERTY(TileGrid, TileGrid, &PlatformerWorld::m_tileGrid)
    REGISTER_PROPERTY(Engine::Color, StaticTileColor, &PlatformerWorld::m_staticTileColor)
    REGISTER_PROPERTY(Engine::Color, DeathTileColor, &PlatformerWorld::m_deathTileColor)
END_TYPE_REGISTER()

TileType PlatformerWorld::TileAt(const Engine::Vec2 &cell) const
{
    return m_tileGrid.At(static_cast<int>(cell.x), static_cast<int>(cell.y));
}

bool PlatformerWorld::IsSolid(const Engine::Vec2 &cell) const
{
    return TileAt(cell) == TileType::Solid;
}

bool PlatformerWorld::IsNextLevel(const Engine::Vec2 &cell) const
{
    return TileAt(cell) == TileType::NextLevel;
}

bool PlatformerWorld::IsDeadly(const Engine::Vec2 &cell) const
{
    return TileAt(cell) == TileType::Death;
}

bool PlatformerWorld::IsPreviousLevel(const Engine::Vec2 &cell) const
{
    return TileAt(cell) == TileType::PreviousLevel;
}

bool PlatformerWorld::OverlapsSolid(const Engine::Rect &box) const
{
    Engine::CellRange range = m_coordSystem->GetSweptCellRange(box, Engine::Vec2::Zero);

    for (int y = static_cast<int>(range.minCell.y); y <= static_cast<int>(range.maxCell.y); ++y)
    {
        for (int x = static_cast<int>(range.minCell.x); x <= static_cast<int>(range.maxCell.x); ++x)
        {
            Engine::Vec2 cell(x, y);
            if (IsSolid(cell) && box.Overlaps(m_coordSystem->GetCellRect(cell)))
                return true;
        }
    }
    return false;
}

bool PlatformerWorld::TouchesSolid(const Engine::Rect &box, const Engine::Vec2 &direction) const
{
    // A hairline sweep in `direction`: SweepRectVsRect requires genuine (not just
    // boundary-touching) overlap on the axis perpendicular to `direction` to register a hit, so
    // solid on a different side of the box - e.g. a wall beside it, while probing below - can't
    // be mistaken for something directly ahead.
    return SweepSolid(box, direction * kContactEpsilon).hit;
}

Engine::SweepHit PlatformerWorld::SweepSolid(const Engine::Rect &movingRect, const Engine::Vec2 &delta) const
{
    Engine::CellRange range = m_coordSystem->GetSweptCellRange(movingRect, delta);

    Engine::SweepHit bestHit;
    for (int y = static_cast<int>(range.minCell.y); y <= static_cast<int>(range.maxCell.y); ++y)
    {
        for (int x = static_cast<int>(range.minCell.x); x <= static_cast<int>(range.maxCell.x); ++x)
        {
            Engine::Vec2 cell(x, y);
            if (!IsSolid(cell))
                continue;

            Engine::SweepHit hit = Engine::SweepRectVsRect(movingRect, delta, m_coordSystem->GetCellRect(cell));
            if (hit.hit && hit.t < bestHit.t)
                bestHit = hit;
        }
    }
    return bestHit;
}

// DDA ray march (Amanatides-Woo style, one cell boundary at a time) to find the first solid
// tile along `direction`, then hands off to SweepRectVsRect for the exact hit point/normal -
// the DDA is only the broad phase here, deciding which single cell to test.
Engine::SweepHit PlatformerWorld::RaycastSolid(const Engine::Vec2 &origin, const Engine::Vec2 &direction, float maxDistance) const
{
    Engine::SweepHit result;
    if (direction.x == 0.0f && direction.y == 0.0f)
        return result;

    Engine::Vec2 cell = m_coordSystem->GetCellFromGridPosition(origin);
    if (IsSolid(cell))
        return result; // Starting inside solid ground - no valid hit

    constexpr float kInfinity = std::numeric_limits<float>::infinity();

    int colStep = direction.x > 0.0f ? 1 : (direction.x < 0.0f ? -1 : 0);
    int rowStep = direction.y > 0.0f ? 1 : (direction.y < 0.0f ? -1 : 0);

    float tPerCol = colStep == 0 ? kInfinity : 1.0f / std::abs(direction.x);
    float tPerRow = rowStep == 0 ? kInfinity : 1.0f / std::abs(direction.y);

    float tToNextCol = colStep == 0 ? kInfinity : ((cell.x + colStep * 0.5f) - origin.x) / direction.x;
    float tToNextRow = rowStep == 0 ? kInfinity : ((cell.y + rowStep * 0.5f) - origin.y) / direction.y;

    while (true)
    {
        float tNextBoundary = std::min(tToNextCol, tToNextRow);
        if (tNextBoundary > maxDistance)
            return result; // Nothing solid within range

        if (tToNextCol < tToNextRow)
        {
            cell.x += colStep;
            tToNextCol += tPerCol;
        }
        else
        {
            cell.y += rowStep;
            tToNextRow += tPerRow;
        }

        if (!m_coordSystem->IsInBounds(cell))
            return result;

        if (!IsSolid(cell))
            continue;

        Engine::Rect rayPoint(origin, Engine::Vec2::Zero);
        return Engine::SweepRectVsRect(rayPoint, direction * maxDistance, m_coordSystem->GetCellRect(cell));
    }
}

Engine::Vec2 PlatformerWorld::FindEntrySpawn(int row) const
{
    for (int x = 0; x < m_tileGrid.GetColumnCount(); ++x)
    {
        if (m_tileGrid.At(x, row) == TileType::EntrySpawn)
            return Engine::Vec2(static_cast<float>(x), static_cast<float>(row));
    }
    return FindDefaultSpawn();
}

Engine::Vec2 PlatformerWorld::FindReturnSpawn(int row) const
{
    for (int x = 0; x < m_tileGrid.GetColumnCount(); ++x)
    {
        if (m_tileGrid.At(x, row) == TileType::ReturnSpawn)
            return Engine::Vec2(static_cast<float>(x), static_cast<float>(row));
    }
    return FindDefaultSpawn();
}

Engine::Vec2 PlatformerWorld::FindDefaultSpawn() const
{
    for (int y = 0; y < m_tileGrid.GetRowCount(); ++y)
        for (int x = 0; x < m_tileGrid.GetColumnCount(); ++x)
            if (m_tileGrid.At(x, y) == TileType::DefaultSpawn)
                return Engine::Vec2(static_cast<float>(x), static_cast<float>(y));

    for (int y = 0; y < m_tileGrid.GetRowCount(); ++y)
        for (int x = 0; x < m_tileGrid.GetColumnCount(); ++x)
            if (m_tileGrid.At(x, y) == TileType::EntrySpawn)
                return Engine::Vec2(static_cast<float>(x), static_cast<float>(y));

    return Engine::Vec2(1.0f, 1.0f);
}

void PlatformerWorld::RenderTile(int x, int y, const Engine::Vec2 &worldPos, const Engine::Vec2 &worldSize) const
{
    switch (m_tileGrid.At(x, y))
    {
    case TileType::Solid:
        Engine::Renderer2D::DrawTile(worldPos, worldSize, m_staticTileColor);
        break;
    case TileType::Death:
        Engine::Renderer2D::DrawTile(worldPos, worldSize, m_deathTileColor);
        break;
    default:
        break;
    }
}
