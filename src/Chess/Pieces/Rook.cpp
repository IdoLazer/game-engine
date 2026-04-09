#include "Rook.h"

void Rook::Initialize()
{
    Entity::Initialize();
    m_hasMoved = false;
}

void Rook::Render() const
{
    // Render the knight as a rectangle with merlons
    // First, render the base rectangle
    ChessPiece::Render();

    // Then, render the merlons on top
    float merlonWidth = m_worldSize.x / 5.0f;
    float embrasureWidth = m_worldSize.x / 5.0f;
    float merlonHeight = m_worldSize.y / 5.0f;
    for (int i = 0; i < 3; ++i)
    {
        Vec2 merlonPos = m_worldPosition + Vec2(
                                               -(m_worldSize.x / 2.0f) + (i * (merlonWidth + embrasureWidth) + merlonWidth / 2.0f),
                                               (m_worldSize.y / 2.0f) + (merlonHeight / 2.0f));
        Renderer2D::DrawTile(merlonPos, Vec2(merlonWidth, merlonHeight), m_color);
    }
}

std::vector<Vec2> Rook::GetPossibleMoves() const
{
    return GetSlidingMoves({
        ChessConstants::DIRECTION_E, ChessConstants::DIRECTION_W,
        ChessConstants::DIRECTION_N, ChessConstants::DIRECTION_S
    });
}

void Rook::OnMove(const Vec2 &newPosition)
{
    ChessPiece::OnMove(newPosition);
    m_hasMoved = true;
}
