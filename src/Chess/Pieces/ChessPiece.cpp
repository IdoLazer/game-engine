#include "ChessPiece.h"

void ChessPiece::Select()
{
    ChessTile *tile = m_board->GetTile(m_gridPosition);
    if (tile)
    {
        tile->ToggleHighlight(true);
    }
    for (auto &cell : GetPossibleMoves())
    {
        ChessTile *moveTile = m_board->GetTile(cell);
        if (moveTile)
        {
            moveTile->ToggleHighlight(true);
        }
    }
}

void ChessPiece::Deselect()
{
    ChessTile *tile = m_board->GetTile(m_gridPosition);
    if (tile)
    {
        tile->ToggleHighlight(false);
    }
    for (auto &cell : GetPossibleMoves())
    {
        ChessTile *moveTile = m_board->GetTile(cell);
        if (moveTile)
        {
            moveTile->ToggleHighlight(false);
        }
    }
}

std::vector<Vec2> ChessPiece::GetSlidingMoves(const std::vector<Vec2> &directions) const
{
    std::vector<Vec2> moves;
    for (const Vec2 &dir : directions)
    {
        Vec2 pos = m_gridPosition + dir;
        while (m_board->IsValidPosition(pos))
        {
            moves.push_back(pos);
            pos += dir;
        }
        // Check if the blocking piece is capturable
        if (m_board->IsOccupied(pos))
        {
            if (m_board->GetPieceAt(pos)->GetPieceColor() != m_pieceColor)
                moves.push_back(pos);
        }
    }
    return moves;
}

std::vector<Vec2> ChessPiece::GetSteppingMoves(const std::vector<Vec2> &directions) const
{
    std::vector<Vec2> moves;
    for (const Vec2 &dir : directions)
    {
        Vec2 pos = m_gridPosition + dir;
        if (m_board->IsValidPosition(pos))
        {
            moves.push_back(pos);
        }
        else if (m_board->IsOccupied(pos))
        {
            if (m_board->GetPieceAt(pos)->GetPieceColor() != m_pieceColor)
                moves.push_back(pos);
        }
    }
    return moves;
}
