#include "Falcon.h"
#include "Player.h"
#include "Cursor.h"
#include "PlatformerWorld.h"
#include "Commands/FalconCommands.h"
#include "StateMachine/FalconStates.h"

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

    RegisterState<OnShoulderState>();
    RegisterState<AimingState>(FalconStateId::OnShoulder);
    RegisterState<FlyingState>();
    RegisterState<ReturningState>();
    RegisterState<LatchedState>();
    RegisterState<GlidingState>();

    if (m_player)
    {
        SetGridPosition(ShoulderPosition());
    }

    m_stateMachine.TransitionTo(FalconStateId::OnShoulder);
}

void Falcon::Update(float deltaTime)
{
    m_stateMachine.Update(deltaTime);
}

void Falcon::Render() const
{
    m_stateMachine.Render();
}

// --- Public Interface ---

void Falcon::StartAiming()
{
    if (!m_stateMachine.AimRequested())
    {
        QueueAimRequest();
    }
}

void Falcon::ReleaseAiming()
{
    ClearQueuedAimRequest();
    m_stateMachine.AimReleased();
}

void Falcon::Retrieve()
{
    m_stateMachine.RetrieveRequested();
}

bool Falcon::IsOnShoulder() const
{
    return m_stateMachine.IsOnShoulder();
}

void Falcon::StartGlide()
{
    m_stateMachine.GlideRequested();
}

void Falcon::StopGlide()
{
    m_stateMachine.GlideReleased();
}

// --- Flight & Aiming ---

Vec2 Falcon::ShoulderPosition() const
{
    return m_player ? m_player->GetGridPosition() + m_offsetFromPlayer : GetGridPosition();
}

void Falcon::FollowPlayer(const Engine::Vec2 &offset)
{
    if (!m_player) return;
    SetGridPosition(m_player->GetGridPosition() + offset);
}

void Falcon::AimAtCursor()
{
    if (!m_cursor || !m_grid) return;
    SetAimPoint(m_grid->WorldToGrid(m_cursor->GetWorldPosition()));
}

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
bool Falcon::FlyTowards(const Engine::Vec2 &target, float speed, float deltaTime)
{
    m_direction = (target - GetGridPosition()).Normalized();
    SetGridPosition(GetGridPosition() + m_direction * speed * deltaTime);

    if ((GetGridPosition() - target).Length() < m_snapRadius)
    {
        SetGridPosition(target); // Snap to the target
        return true;
    }
    return false;
}

// --- Deferred Aim ---

void Falcon::QueueAimRequest()
{
    if (m_startAimingCommandQueue.HasCommands()) return;
    m_startAimingCommandQueue.EnqueueCommand(std::make_unique<StartAimingCommand>(*this));
}

void Falcon::ConsumeQueuedAimRequest()
{
    if (m_startAimingCommandQueue.HasCommands())
    {
        m_startAimingCommandQueue.DequeueCommand()->Execute();
    }
}

void Falcon::ClearQueuedAimRequest()
{
    m_startAimingCommandQueue.Clear();
}

// --- Rendering ---

// A triangle with its point in the direction the falcon is facing.
void Falcon::DrawBody() const
{
    Vec2 worldCenter = GetWorldPosition();
    Vec2 worldSize = GetWorldSize();
    Vec2 worldDirection = Vec2(m_direction.x, -m_direction.y).Normalized(); // Invert y for rendering

    // Falcon's "face" - the point in the direction it's facing
    Vec2 p1 = worldCenter + worldDirection * (worldSize.x / 2.0f);

    // The two other points are in the opposite direction, forming a triangle
    Vec2 perp = Vec2(-worldDirection.y, worldDirection.x).Normalized(); // Perpendicular vector to the direction
    Vec2 p2 = worldCenter - worldDirection * (worldSize.x / 2.0f) + perp * (worldSize.y / 2.0f);
    Vec2 p3 = worldCenter - worldDirection * (worldSize.x / 2.0f) - perp * (worldSize.y / 2.0f);

    Renderer2D::DrawTriangle(p1, p2, p3, m_color);
}

// Wings spread: two triangles in a fixed rotation, not one pointing where it faces.
void Falcon::DrawGlidingBody() const
{
    Vec2 worldCenter = GetWorldPosition();
    Vec2 worldSize = GetWorldSize();

    // Triangle 1 (left wing)
    Vec2 p1 = worldCenter + Vec2(-3.0f, 0.5f) * (worldSize.x / 2.0f);
    Vec2 p2 = worldCenter + Vec2(0.0f, 1.0f) * (worldSize.y / 2.0f);
    Vec2 p3 = worldCenter + Vec2(0.0f, -1.0f) * (worldSize.y / 2.0f);
    Renderer2D::DrawTriangle(p1, p2, p3, m_color);

    // Triangle 2 (right wing)
    p1 = worldCenter + Vec2(3.0f, 0.5f) * (worldSize.x / 2.0f);
    Renderer2D::DrawTriangle(p1, p2, p3, m_color);
}

// The line out to the aim point, marking where a release would latch.
void Falcon::DrawAimOverlay() const
{
    Vec2 worldAimPoint = m_grid->GridToWorld(m_aimPoint);

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
