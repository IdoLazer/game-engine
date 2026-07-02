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

// DDA ray-cast from `origin` along `direction`, stopping at `m_goal`.
// Returns true if the ray hits a solid tile through its bottom face before reaching the goal.
// A side hit or no hit both return false — only ceiling latch is valid.
bool Falcon::FindLatchTarget(const Engine::Vec2 &origin, const Engine::Vec2 &direction, Engine::Vec2 &latchPoint) const
{
    if (!m_world || direction.y >= 0.0f)
        return false;

    Vec2 cell = GetGrid()->GetCellFromGridPosition(origin);
    if (m_world->IsSolid(cell))
        return false;

    constexpr float kInfinity = std::numeric_limits<float>::infinity();

    // How many columns to step per iteration (+1 right, -1 left, 0 straight up)
    int colStep = direction.x > 0.0f ? 1 : (direction.x < 0.0f ? -1 : 0);
    // How many rows to step per iteration (-1 up, only name for clarity)
    int rowStep = -1;

    // How much t increases to cross one full cell on each axis
    float tPerCol = colStep == 0 ? kInfinity : 1.0f / std::abs(direction.x);
    float tPerRow = rowStep == 0 ? kInfinity : 1.0f / std::abs(direction.y);

    // t at which the ray first crosses the next boundary on each axis
    float tToNextCol = colStep == 0 ? kInfinity : ((cell.x + colStep * 0.5f) - origin.x) / direction.x;
    float tToNextRow = rowStep == 0 ? kInfinity : ((cell.y + rowStep * 0.5f) - origin.y) / direction.y;

    // Don't march past the goal
    float tGoal = (m_goal - origin).Length();

    while (true)
    {
        float tNextBoundary = std::min(tToNextCol, tToNextRow);
        if (tNextBoundary > tGoal)
            return false;

        bool enteredFromSide = tToNextCol < tToNextRow;
        if (enteredFromSide)
        {
            cell.x += colStep;
            tToNextCol += tPerCol;
        }
        else
        {
            cell.y += rowStep;
            tToNextRow += tPerRow;
        }

        if (!GetGrid()->IsInBounds(cell))
            return false;

        if (!m_world->IsSolid(cell))
            continue;

        // We've hit a solid tile. Determine if it was a side hit or a bottom hit.
        
        if (enteredFromSide)
            return false; // Side hit — not a valid ceiling latch

        // Bottom hit — compute the exact point on the tile's underside
        float tileUndersideY = cell.y + 0.5f;
        float tHit           = (tileUndersideY - origin.y) / direction.y;
        latchPoint           = origin + direction * tHit;
        return true;
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
