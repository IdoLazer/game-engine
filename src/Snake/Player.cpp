#include "Player.h"
#include "Commands/MovementInputHandler.h"

// --- Type Registration ---

BEGIN_TYPE_REGISTER(Player)
    REGISTER_PROPERTY(Engine::Vec2, Direction, &Player::m_direction)
    REGISTER_PROPERTY(int, InitialTailLength, &Player::m_initialTailLength)
    REGISTER_PROPERTY(float, MoveSpeed, &Player::m_moveSpeed)
END_TYPE_REGISTER()

using namespace Engine;

// --- Lifecycle ---

void Player::Initialize()
{
    // Call parent to sync grid→world and register with grid
    GridEntity::Initialize();

    // Set up movement timer
    m_moveTimer = Timer{1.0f / m_moveSpeed, [this]()
                        { Move(); }, true};

    // Create initial tail segments
    m_tailSegments.reserve(m_initialTailLength);
    for (int i = 0; i < m_initialTailLength; ++i)
    {
        Vec2 tailPosition = Vec2(m_gridPosition.x - (i + 1) * m_direction.x, m_gridPosition.y - (i + 1) * m_direction.y);
        GridTile gridTile;
        gridTile.SetGrid(m_grid);
        gridTile.SetGridSize(m_gridSize);
        gridTile.SetColor(m_color);
        gridTile.SetGridPosition(tailPosition);
        m_tailSegments.emplace_back(gridTile);
    }
}

void Player::Destroy()
{
    m_tailSegments.clear();
}

void Player::Update(float deltaTime)
{
    m_moveTimer.Update(deltaTime);
}

void Player::Render() const
{
    // Draw Player head using inherited world coordinates (automatically synced from grid position)
    Renderer2D::DrawTile(m_worldPosition, m_worldSize, m_color);

    // Draw tail segments
    for (const GridTile &segment : m_tailSegments)
    {
        segment.Render();
    }
}

// --- Game Logic ---

void Player::Move()
{
    // Process any queued commands before moving
    ProcessQueuedCommand();

    if (m_growTailOnNextMove)
    {
        GridTile newSegment;
        newSegment.SetGrid(m_grid);
        newSegment.SetGridSize(m_gridSize);
        newSegment.SetColor(m_color);
        newSegment.SetGridPosition(m_gridPosition);
        m_tailSegments.push_back(newSegment);
        m_growTailOnNextMove = false;
    }
    // Move the tail segments by shifting them forward and placing the previous head position at the front
    if (!m_tailSegments.empty())
    {
        std::rotate(m_tailSegments.rbegin(), m_tailSegments.rbegin() + 1, m_tailSegments.rend());
        m_tailSegments.front().SetGridPosition(m_gridPosition);
    }

    // Update grid position (this automatically updates world position via inheritance)
    SetGridPosition(m_gridPosition + m_direction);
    m_updateMoveThisFrame = false;
}

void Player::Grow()
{
    m_growTailOnNextMove = true;
}

bool Player::CheckSelfCollision() const
{
    // Check if the player has collided with its own tail
    for (const GridTile &segment : m_tailSegments)
    {
        if (m_gridPosition == segment.GetGridPosition())
            return true;
    }
    return false;
}

// --- Accessors ---

const std::vector<GridTile> &Player::GetTailSegments() const { return m_tailSegments; }
Vec2 Player::GetDirection() const { return m_direction; }
void Player::SetInputHandler(MovementInputHandler *handler) { m_inputHandler = handler; }
void Player::SetInitialTailLength(int length) { m_initialTailLength = length; }
void Player::SetMoveSpeed(float speed) { m_moveSpeed = speed; }

void Player::SetDirection(const Vec2 &dir)
{
    // Only set direction if it won't cause immediate reversal
    if (m_direction == -dir)
        return;
    m_updateMoveThisFrame = true;
    m_direction = dir;
}

// --- Internal ---

void Player::ProcessQueuedCommand()
{
    if (m_inputHandler && m_inputHandler->HasQueuedMovementCommands())
    {
        auto command = m_inputHandler->GetNextMovementCommand();
        if (command)
        {
            command->Execute();
        }
    }
}
