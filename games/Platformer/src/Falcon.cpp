#include "Falcon.h"
#include "Player.h"
#include "Cursor.h"
#include "PlatformerWorld.h"

using namespace Engine;

BEGIN_TYPE_REGISTER(Falcon)
    REGISTER_PROPERTY(Engine::Vec2, OffsetFromPlayer, &Falcon::m_offsetFromPlayer)
    REGISTER_PROPERTY(float, Speed, &Falcon::m_speed)
    REGISTER_PROPERTY(float, SnapRadius, &Falcon::m_snapRadius)
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
    switch (m_state)
    {
    case FalconState::OnShoulder:
        // Update the falcon's position to follow the player with the specified offset
        if (m_player)
        {
            SetGridPosition(m_player->GetGridPosition() + m_offsetFromPlayer);
        }
        if (m_isAiming)
        {
            // If aiming, set the aim point to the cursor's position
            SetAimPoint(m_grid->WorldToGrid(m_cursor ? m_cursor->GetWorldPosition() : GetWorldPosition()));
        }
        break;
    case FalconState::Flying:
        if (m_latchPoint && FlyTowards(*m_latchPoint, deltaTime))
        {
            LatchToCeiling();
        }
        break;
    case FalconState::Returning:
        if (m_player && FlyTowards(m_player->GetGridPosition() + m_offsetFromPlayer, deltaTime))
        {
            m_state = FalconState::OnShoulder;
        }
        break;
    case FalconState::Latched:
        break;
    }
}

void Falcon::Render() const
{
    // Convert the falcon's grid position and size to world coordinates for rendering
    Vec2 worldCenter = GetWorldPosition();
    Vec2 worldSize = GetWorldSize();
    Vec2 worldAimPoint = m_grid->GridToWorld(m_aimPoint);
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
        // Draw a line from the falcon to its aim point for debugging purposes
        if (m_latchPoint)
        {
            Vec2 latchWorldPos = m_grid->GridToWorld(*m_latchPoint);
            Renderer2D::DrawLine(worldCenter, latchWorldPos, Color::White, 0.02f, LineStyle::Dashed);
            Renderer2D::DrawLine(latchWorldPos, worldAimPoint, Color::Grey, 0.02f, LineStyle::Dashed);

            // Draw the latch point as a white x if the aim point is latchable
            float latchSize = 0.06f;
            Renderer2D::DrawLine(latchWorldPos - Vec2(latchSize, latchSize), latchWorldPos + Vec2(latchSize, latchSize), Color::White, 0.05f);
            Renderer2D::DrawLine(latchWorldPos - Vec2(-latchSize, latchSize), latchWorldPos + Vec2(-latchSize, latchSize), Color::White, 0.05f);
        }
        else
        {
            Renderer2D::DrawLine(worldCenter, worldAimPoint, Color::Grey, 0.02f, LineStyle::Dashed);
        }
    }
}

// --- Public Interface ---

void Falcon::StartAiming()
{
    if (m_state != FalconState::OnShoulder) return;

    m_isAiming = true;
}

void Falcon::ReleaseAiming()
{
    if (m_state != FalconState::OnShoulder) return;

    m_isAiming = false;
    if (m_latchPoint)
    {
        m_state = FalconState::Flying;
    }
}

void Falcon::Retrieve()
{
    if (m_state == FalconState::OnShoulder) return;
    m_state = FalconState::Returning;
    m_latchPoint.reset();
}

// --- Falcon Behavior ---

void Falcon::SetAimPoint(const Engine::Vec2 &aimPoint)
{
    m_aimPoint = aimPoint;
    Vec2 origin = GetGridPosition();
    m_direction = (aimPoint - origin).Normalized();

    if (!m_world || !m_player)
    {
        m_latchPoint.reset();
        return;
    }

    // Cast from the player's position - the falcon can be embedded in a wall it's offset from.
    Vec2 playerOrigin = m_player->GetGridPosition();
    Vec2 toAimPoint = aimPoint - playerOrigin;
    float maxDistance = toAimPoint.Length();
    Vec2 rayDirection = toAimPoint.Normalized();
    SweepHit hit = m_world->RaycastSolid(playerOrigin, rayDirection, maxDistance);

    // Only a bottom-face hit is a valid ceiling latch - side hits don't count.
    if (hit.hit && hit.normal.y > 0.0f)
    {
        m_latchPoint = playerOrigin + rayDirection * (hit.t * maxDistance);
    }
    else
    {
        m_latchPoint.reset();
    }
}

// Steers towards target, moves, and snaps on arrival. Returns true once arrived.
// Deliberately uncollided in both directions - see FUTURE.md.
bool Falcon::FlyTowards(const Engine::Vec2 &target, float deltaTime)
{
    m_direction = (target - GetGridPosition()).Normalized();
    SetGridPosition(GetGridPosition() + m_direction * m_speed * deltaTime);

    if ((GetGridPosition() - target).Length() < m_snapRadius)
    {
        SetGridPosition(target); // Snap to the target
        return true;
    }
    return false;
}

// Falcon becomes a fixed hinge point, pointy end stuck straight up into the tile.
void Falcon::LatchToCeiling()
{
    m_direction = Vec2(0.0f, -1.0f); // Snap to vertical - stuck pointing straight up
    m_state = FalconState::Latched;
}
