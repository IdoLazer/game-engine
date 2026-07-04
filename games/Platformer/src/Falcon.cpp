#include "Falcon.h"
#include "Player.h"
#include "Cursor.h"
#include "PlatformerWorld.h"

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

    if (!m_world || !m_player)
    {
        m_isGoalLatchable = false;
        return;
    }

    // Cast from the player's position - the falcon can be embedded in a wall it's offset from.
    Vec2 playerOrigin = m_player->GetGridPosition();
    Vec2 toGoal = goal - playerOrigin;
    float maxDistance = toGoal.Length();
    Vec2 rayDirection = toGoal.Normalized();
    SweepHit hit = m_world->RaycastSolid(playerOrigin, rayDirection, maxDistance);

    // Only a bottom-face hit is a valid ceiling latch - side hits don't count.
    m_isGoalLatchable = hit.hit && hit.normal.y > 0.0f;
    if (m_isGoalLatchable)
    {
        m_latchPoint = playerOrigin + rayDirection * (hit.t * maxDistance);
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
    // Deliberately uncollided - see FUTURE.md.
    SetGridPosition(GetGridPosition() + m_direction * m_speed * deltaTime);

    // Check if the falcon has reached the player
    if ((GetGridPosition() - m_goal).Length() < 0.1f)
    {
        m_isReturningToPlayer = false;
        m_isFollowingPlayer = true;
        SetGridPosition(m_goal); // Snap to the player's position
    }
}

// Uncollided - SetGoal already validated the path via raycast at aim time.
void Falcon::MoveToGoal(float deltaTime)
{
    m_direction = (m_latchPoint - GetGridPosition()).Normalized();
    SetGridPosition(GetGridPosition() + m_direction * m_speed * deltaTime);

    if ((GetGridPosition() - m_latchPoint).Length() < 0.1f)
    {
        SetGridPosition(m_latchPoint);
        LatchToCeiling();
    }
}

// Falcon becomes a fixed hinge point, pointy end stuck straight up into the tile.
void Falcon::LatchToCeiling()
{
    m_direction = Vec2(0.0f, -1.0f); // Snap to vertical - stuck pointing straight up
    m_isMovingToGoal = false;
    m_isLatched = true;
}
