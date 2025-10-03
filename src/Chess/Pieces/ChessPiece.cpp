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
