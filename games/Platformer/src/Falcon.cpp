#include "Falcon.h"
#include "Player.h"
#include "Cursor.h"
#include "PlatformerWorld.h"
#include <cmath>
#include <limits>

using namespace Engine;

BEGIN_TYPE_REGISTER(Falcon)
    REGISTER_PROPERTY(Engine::Vec2, OffsetFromPlayer, &Falcon::m_offsetFromPlayer)
    REGISTER_PROPERTY(float, Speed, &Falcon::m_speed)
END_TYPE_REGISTER()

// --- Lifecycle ---

void Falcon::Initialize()
{
    GridEntity::Initialize();

    // Set the falcon's initial position relative to the player
    if (m_player)
    {
        SetGridPosition(m_player->GetGridPosition() + m_offsetFromPlayer);
    }
}

void Falcon::Update(float deltaTime)
{
    // State 1: On Player's Shoulder (following the player)
    if (m_isFollowingPlayer)
    {
        // Update the falcon's position to follow the player with the specified offset
        if (m_player)
        {
            SetGridPosition(m_player->GetGridPosition() + m_offsetFromPlayer);
        }
        if (m_isAiming)
        {
            // If aiming, set the goal to the cursor's position
            SetGoal(m_grid->WorldToGrid(m_cursor ? m_cursor->GetWorldPosition() : GetWorldPosition()));
        }
    }
    // State 2: Moving to Goal Position
    else if (m_isMovingToGoal)
    {
        MoveToGoal(deltaTime);
        Vec2 newPos = GetGridPosition();
        if (TryLatchToCeiling(newPos))
        {
            SetGridPosition(newPos);
        }
    }
    // State 3: Returning to Player
    else if (m_isReturningToPlayer)
    {
        ReturnToPlayer(deltaTime);
    }
}

void Falcon::Render() const
{
    // Convert the falcon's grid position and size to world coordinates for rendering
    Vec2 worldCenter = GetWorldPosition();
    Vec2 worldSize = GetWorldSize();
    Vec2 worldGoal = m_grid->GridToWorld(m_goal);
    Vec2 worldDirection = Vec2(m_direction.x, -m_direction.y).Normalized(); // Invert y for rendering
    
    // Draw falcon as a white triangle pointing in the direction it's facing

    // Falcon's "face" - the point in the direction it's facing
    Vec2 p1 = worldCenter + worldDirection * (worldSize.x / 2.0f);
    
    // The two other points are in the opposite direction, forming a triangle
    Vec2 perp = Vec2(-worldDirection.y, worldDirection.x).Normalized(); // Perpendicular vector to the direction
    Vec2 p2 = worldCenter - worldDirection * (worldSize.x / 2.0f) + perp * (worldSize.y / 2.0f);
    Vec2 p3 = worldCenter - worldDirection * (worldSize.x / 2.0f) - perp * (worldSize.y / 2.0f);

    Renderer2D::DrawTriangle(p1, p2, p3, m_color);

    if (m_isAiming)
    {
        // Draw a line from the falcon to its goal position for debugging purposes
        if (m_isGoalLatchable)
        {
            Vec2 latchWorldPos = m_grid->GridToWorld(m_latchPoint);
            Renderer2D::DrawLine(worldCenter, latchWorldPos, Color::White, 0.02f, LineStyle::Dashed);
            Renderer2D::DrawLine(latchWorldPos, worldGoal, Color::Grey, 0.02f, LineStyle::Dashed);

            // Draw the latch point as a white x if the goal is latchable
            float latchSize = 0.06f;
            Renderer2D::DrawLine(latchWorldPos - Vec2(latchSize, latchSize), latchWorldPos + Vec2(latchSize, latchSize), Color::White, 0.05f);
            Renderer2D::DrawLine(latchWorldPos - Vec2(-latchSize, latchSize), latchWorldPos + Vec2(-latchSize, latchSize), Color::White, 0.05f);
        }
        else
        {
            Renderer2D::DrawLine(worldCenter, worldGoal, Color::Grey, 0.02f, LineStyle::Dashed);
        }
    }
}

// --- Public Interface ---

void Falcon::StartAiming()
{
    if (!m_isFollowingPlayer) return;

    m_isAiming = true;
}

void Falcon::ReleaseAiming()
{
    if (!m_isFollowingPlayer) return;

    m_isAiming = false;
    if (m_isGoalLatchable)
    {
        m_isMovingToGoal = true;
        m_isFollowingPlayer = false;
    }
}

void Falcon::Retrieve()
{
    if (m_isFollowingPlayer) return;
    m_isReturningToPlayer = true;
    m_isMovingToGoal = false;
    m_isGoalLatchable = false;
    m_isLatched = false;
}

// --- Falcon Behavior ---

void Falcon::SetGoal(const Engine::Vec2 &goal)
{
    m_goal = goal;
    Vec2 origin = GetGridPosition();
    m_direction = (goal - origin).Normalized();

    // The aimed-at cursor position doesn't necessarily matter - what matters is the first
    // solid tile the ray actually crosses. If that tile is a valid latch target, treat it as
    // the goal instead; otherwise fall back to the raw cursor position for the aim preview.
    Vec2 latchPoint;
    m_isGoalLatchable = FindLatchTarget(origin, m_direction, latchPoint);
    if (m_isGoalLatchable)
    {
        m_latchPoint = latchPoint;
    }
}

// Walks the aim ray cell-by-cell (a grid "DDA" traversal) from `origin` along `direction` until
// it finds the first solid tile. That tile is only a valid latch target if the ray reached it by
// crossing a horizontal cell boundary (i.e. entering through its underside) - if the ray clips a
// solid tile's side first, latching is rejected even if a further tile would've been hit from below.
bool Falcon::FindLatchTarget(const Engine::Vec2 &origin, const Engine::Vec2 &direction, Engine::Vec2 &latchPoint) const
{
    if (!m_world || direction.y >= 0.0f)
    {
        return false; // Must be aiming at least partly upward to ever cross a tile's underside
    }

    Vec2 cell = GetGrid()->GetCellFromGridPosition(origin);
    if (m_world->IsSolid(cell))
    {
        return false; // Already inside a solid tile - can't latch to it
    }
    
    int stepX = direction.x > 0.0f ? 1 : (direction.x < 0.0f ? -1 : 0);
    constexpr float infinity = std::numeric_limits<float>::infinity();

    // tMax*: parametric distance along the ray to the *next* grid line on that axis.
    // tDelta*: parametric distance needed to cross one full cell on that axis.
    // (direction is normalized, so these distances are in grid units.)
    float tMaxX = stepX == 0 ? infinity : ((cell.x + stepX * 0.5f) - origin.x) / direction.x;
    float tMaxY = ((cell.y - 0.5f) - origin.y) / direction.y;
    float tDeltaX = stepX == 0 ? infinity : 1.0f / std::abs(direction.x);
    float tDeltaY = 1.0f / std::abs(direction.y);

    while (true)
    {
        // Advance into whichever neighboring cell the ray reaches first
        bool crossedSide = tMaxX < tMaxY;
        if (crossedSide)
        {
            cell.x += stepX;
            tMaxX += tDeltaX;
        }
        else
        {
            cell.y -= 1.0f;
            tMaxY += tDeltaY;
        }

        if (!GetGrid()->IsInBounds(cell))
        {
            return false; // Left the grid without hitting anything
        }

        if (!m_world->IsSolid(cell))
        {
            continue;
        }

        if (crossedSide)
        {
            return false; // First solid tile was clipped from the side - not a valid latch
        }

        // latch point is the intersection of the ray with the underside of the solid tile
        Vec2 cellBottom = cell + Vec2(0.0f, 0.5f);
        float t = (cellBottom.y - origin.y) / direction.y;
        latchPoint = origin + direction * t;

        return true; // First solid tile was entered through its underside
    }
}

void Falcon::ReturnToPlayer(float deltaTime)
{
    if (!m_player)
    {
        return;
    }

    m_goal = m_player->GetGridPosition() + m_offsetFromPlayer;
    m_direction = (m_goal - GetGridPosition()).Normalized();
    MoveToGoal(deltaTime);

    // Check if the falcon has reached the player
    if ((GetGridPosition() - m_goal).Length() < 0.1f)
    {
        m_isReturningToPlayer = false;
        m_isFollowingPlayer = true;
        SetGridPosition(m_goal); // Snap to the player's position
    }
}

void Falcon::MoveToGoal(float deltaTime)
{
    Vec2 currentPos = GetGridPosition();
    Vec2 newPos = currentPos + m_direction * m_speed * deltaTime;
    SetGridPosition(newPos);
}

// Checks whether the falcon's leading (top) edge has plunged into a solid tile from below.
// If so, it stops there and its "pointy end" sticks straight up into the tile - becoming a
// vertical hinge point instead of arriving exactly at the aimed goal position.
bool Falcon::TryLatchToCeiling(Engine::Vec2 &newPos)
{
    float halfHeight = GetGridSize().y / 2.0f;
    Vec2 topEdgeProbe(newPos.x, newPos.y - halfHeight);
    Vec2 cell = GetGrid()->GetCellFromGridPosition(topEdgeProbe);

    if (!m_world || !m_world->IsSolid(cell))
    {
        return false;
    }

    newPos.y = (cell.y + 0.5f) + halfHeight; // Rest the top edge against the tile's underside
    m_direction = Vec2(0.0f, -1.0f); // Snap to vertical - stuck pointing straight up
    m_isMovingToGoal = false;
    m_isLatched = true;
    return true;
}
