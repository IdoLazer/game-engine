#include "ChessBoard.h"
#include "Pieces/ChessPiece.h"
#include <iostream>

ChessBoard::~ChessBoard()
{
    for (auto tile : m_tiles)
    {
        delete tile;
    }
    for (auto piece : m_pieces)
    {
        delete piece;
    }
}

void ChessBoard::Initialize()
{
    Grid::Initialize();

    Color black = Color(139.0f / 255.0f, 0, 0);                             // Dark Red
    Color white = Color(198.0f / 255.0f, 218.0f / 255.0f, 167.0f / 255.0f); // Beige

    m_tiles.reserve(8 * 8);
    for (int y = 0; y < 8; ++y)
    {
        for (int x = 0; x < 8; ++x)
        {
            Color tileColor = (x + y) % 2 == 0 ? white : black;
            ChessTile *tile = new ChessTile(this, Vec2{x, y}, Vec2{1, 1}, tileColor);
            m_tiles.emplace_back(tile);
        }
    }
}

void ChessBoard::Render()
{
    for (const auto &tile : m_tiles)
    {
        tile->Render();
    }

    for (const auto &piece : m_pieces)
    {
        piece->Render();
    }
}

void ChessBoard::Update(float deltaTime)
{
    Vec2 mousePos = Mouse::GetWorldPosition();
    Vec2 gridPos = WorldToGrid(mousePos);
    if (IsValidPosition(gridPos) && Mouse::IsButtonPressed(GLFW_MOUSE_BUTTON_1))
    {
        OnMouseClick(gridPos);
    }
}

bool ChessBoard::IsValidPosition(const Vec2 &gridPos) const
{
    return gridPos.x >= 0 && gridPos.x < GetCellCount().x && gridPos.y >= 0 && gridPos.y < GetCellCount().y;
}

void ChessBoard::AddPiece(ChessPiece *piece)
{
    m_pieces.push_back(piece);
}

void ChessBoard::OnMouseClick(const Vec2 &gridPos)
{
    // Check if a piece exists at the clicked cell
    Vec2 cell = GetCellFromGridPosition(gridPos);
    for (const auto &piece : m_pieces)
    {
        if (piece->GetGridCell() == cell)
        {
            if (m_selectedPiece && piece != m_selectedPiece)
            {
                // Deselect previous piece
                m_selectedPiece->Deselect();
            }
            m_selectedPiece = piece;
            piece->Select();
            return;
        }
    }
}

ChessTile *ChessBoard::GetTile(const Vec2 &cell) const
{
    if (!IsValidPosition(cell))
        return nullptr;
    int index = static_cast<int>(cell.y) * 8 + static_cast<int>(cell.x);
    if (index >= 0 && index < static_cast<int>(m_tiles.size()))
        return m_tiles[index];
    return nullptr;
}
