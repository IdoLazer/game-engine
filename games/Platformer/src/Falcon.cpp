#include "Falcon.h"
#include "Player.h"
#include "Cursor.h"
#include "PlatformerWorld.h"
#include "Commands/FalconCommands.h"

using namespace Engine;

BEGIN_TYPE_REGISTER(Falcon)
    REGISTER_PROPERTY(Engine::Vec2, OffsetFromPlayer, &Falcon::m_offsetFromPlayer)
    REGISTER_PROPERTY(Engine::Vec2, GlideOffsetFromPlayer, &Falcon::m_glideOffsetFromPlayer)
    REGISTER_PROPERTY(float, Speed, &Falcon::m_speed)
    REGISTER_PROPERTY(float, RetrieveSpeed, &Falcon::m_retrieveSpeed)
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
    case FalconState::Aiming:
        // If aiming, set the aim point to the cursor's position
        if (m_cursor && m_grid)
        {
            SetAimPoint(m_grid->WorldToGrid(m_cursor->GetWorldPosition()));
        }
        // No break here since aiming falcon should also follow the player
    case FalconState::OnShoulder:
        // Update the falcon's position to follow the player with the specified offset
        if (m_player)
        {
            SetGridPosition(m_player->GetGridPosition() + m_offsetFromPlayer);
        }
        break;
    case FalconState::Flying:
        if (m_latchPoint && FlyTowards(*m_latchPoint, deltaTime))
        {
            Latch();
        }
        break;
    case FalconState::Returning:
        if (m_player && FlyTowards(m_player->GetGridPosition() + m_offsetFromPlayer, deltaTime))
        {
            ReturnToShoulder();
        }
        break;
    case FalconState::Latched:
        break;
    case FalconState::Gliding:
        // Update the falcon's position to follow the player with the specified offset
        if (m_player)
        {
            SetGridPosition(m_player->GetGridPosition() + m_glideOffsetFromPlayer);
        }
    }
}

void Falcon::Render() const
{
    // Convert the falcon's grid position and size to world coordinates for rendering
    Vec2 worldCenter = GetWorldPosition();
    Vec2 worldSize = GetWorldSize();
    Vec2 worldAimPoint = m_grid->GridToWorld(m_aimPoint);
    Vec2 worldDirection = Vec2(m_direction.x, -m_direction.y).Normalized(); // Invert y for rendering
    
    if (m_state == FalconState::Gliding)
    {
        // If gliding, the falcon's wings are spread out, so we draw it as 2 triangles in a fixed rotation
        
        // Triangle 1 (left wing)
        Vec2 p1 = worldCenter + Vec2(-3.0f, 0.5f) * (worldSize.x / 2.0f);
        Vec2 p2 = worldCenter + Vec2(0.0f, 1.0f) * (worldSize.y / 2.0f);
        Vec2 p3 = worldCenter + Vec2(0.0f, -1.0f) * (worldSize.y / 2.0f);
        Renderer2D::DrawTriangle(p1, p2, p3, m_color);

        // Triangle 2 (right wing)
        p1 = worldCenter + Vec2(3.0f, 0.5f) * (worldSize.x / 2.0f);
        Renderer2D::DrawTriangle(p1, p2, p3, m_color);

    }
    else
    {
        // Draw falcon as a white triangle pointing in the direction it's facing
    
        // Falcon's "face" - the point in the direction it's facing
        Vec2 p1 = worldCenter + worldDirection * (worldSize.x / 2.0f);
        
        // The two other points are in the opposite direction, forming a triangle
        Vec2 perp = Vec2(-worldDirection.y, worldDirection.x).Normalized(); // Perpendicular vector to the direction
        Vec2 p2 = worldCenter - worldDirection * (worldSize.x / 2.0f) + perp * (worldSize.y / 2.0f);
        Vec2 p3 = worldCenter - worldDirection * (worldSize.x / 2.0f) - perp * (worldSize.y / 2.0f);
    
        Renderer2D::DrawTriangle(p1, p2, p3, m_color);
    }

    if (m_state == FalconState::Aiming)
    {
        // Draw a line from the player to the aim point for debugging purposes
        if (m_latchPoint)
        {
            Vec2 latchWorldPos = m_grid->GridToWorld(*m_latchPoint);
            Renderer2D::DrawLine(m_player->GetWorldPosition(), latchWorldPos, Color::White, 0.02f, LineStyle::Dashed);
            Renderer2D::DrawLine(latchWorldPos, worldAimPoint, Color::Grey, 0.02f, LineStyle::Dashed);

            // Draw the latch point as a white x if the aim point is latchable
            float latchSize = 0.06f;
            Renderer2D::DrawLine(latchWorldPos - Vec2(latchSize, latchSize), latchWorldPos + Vec2(latchSize, latchSize), Color::White, 0.05f);
            Renderer2D::DrawLine(latchWorldPos - Vec2(-latchSize, latchSize), latchWorldPos + Vec2(-latchSize, latchSize), Color::White, 0.05f);
        }
        else
        {
            Renderer2D::DrawLine(m_player->GetWorldPosition(), worldAimPoint, Color::Grey, 0.02f, LineStyle::Dashed);
        }
    }
}

// --- Public Interface ---

void Falcon::StartAiming()
{
    if (m_state != FalconState::OnShoulder)
    {
        if (!m_startAimingCommandQueue.HasCommands())
        {
            m_startAimingCommandQueue.EnqueueCommand(std::make_unique<StartAimingCommand>(*this));
        }
        return;
    }

    m_state = FalconState::Aiming;
}

void Falcon::ReleaseAiming()
{
    m_startAimingCommandQueue.Clear();

    if (m_state != FalconState::Aiming) return;

    if (m_latchPoint)
    {
        m_state = FalconState::Flying;
    }
    else
    {
        ReturnToShoulder();
    }
}

void Falcon::Retrieve()
{
    if (!(m_state == FalconState::Flying || m_state == FalconState::Latched)) return;
    m_state = FalconState::Returning;
    m_latchPoint.reset();
}

bool Falcon::IsOnShoulder() const
{
    return m_state == FalconState::OnShoulder || m_state == FalconState::Aiming;
}

void Falcon::StartGlide()
{
    if (m_state == FalconState::Aiming && !m_startAimingCommandQueue.HasCommands())
    {
        m_startAimingCommandQueue.EnqueueCommand(std::make_unique<StartAimingCommand>(*this));
    }

    if (m_state == FalconState::OnShoulder || m_state == FalconState::Aiming)
    {
        m_state = FalconState::Gliding;
    }
}

void Falcon::StopGlide()
{
    if (m_state == FalconState::Gliding)
    {
        ReturnToShoulder();
    }
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

    if (hit.hit)
    {
        m_latchPoint = playerOrigin + rayDirection * (hit.t * maxDistance);
        // hit.normal is one of the four cardinal directions (tiles are grid-aligned) and
        // points back the way we came, so its negation is the direction to embed into the surface.
        m_latchDirection = -hit.normal;
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
    float speed = (m_state == FalconState::Returning) ? m_retrieveSpeed : m_speed;
    SetGridPosition(GetGridPosition() + m_direction * speed * deltaTime);

    if ((GetGridPosition() - target).Length() < m_snapRadius)
    {
        SetGridPosition(target); // Snap to the target
        return true;
    }
    return false;
}

// Falcon becomes a fixed hinge point, pointy end stuck into whichever surface it hit.
void Falcon::Latch()
{
    m_direction = m_latchDirection;
    m_state = FalconState::Latched;
}

void Falcon::ReturnToShoulder()
{
    m_state = FalconState::OnShoulder;
    if (m_startAimingCommandQueue.HasCommands())
    {
        m_startAimingCommandQueue.DequeueCommand()->Execute();
    }
    m_onReturnedEvent.Notify();
}
