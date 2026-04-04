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
    std::vector<Vec2> possibleMoves;
    Vec2 directions[] = {Vec2(1, 0), Vec2(-1, 0), Vec2(0, 1), Vec2(0, -1)}; // Right, Left, Up, Down
    for (const Vec2 &dir : directions)
    {
        Vec2 currentPos = m_gridPosition + dir;
        while (m_board->IsValidPosition(currentPos))
        {
            possibleMoves.push_back(currentPos);
            currentPos += dir;
        }
        if (m_board->IsOccupied(currentPos))
        {
            ChessPiece *pieceAtPos = m_board->GetPieceAt(currentPos);
            if (pieceAtPos->GetPieceColor() != m_ChessPieceColor)
            {
                possibleMoves.push_back(currentPos); // Can capture opponent's piece
            }
        }
    }

    return possibleMoves;
}

void Rook::OnMove(const Vec2 &newPosition)
{
    ChessPiece::OnMove(newPosition);
    m_hasMoved = true;
}
