#include "ChessBoard.h"
#include "Pieces/ChessPiece.h"
#include <iostream>

ChessBoard::~ChessBoard()
{
    for (auto tile : m_tiles)
    {
        delete tile;
    }
    for (auto piece : m_whitePieces)
    {
        delete piece;
    }
    for (auto piece : m_blackPieces)
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

    for (const auto &piece : m_whitePieces)
    {
        piece->Render();
    }

    for (const auto &piece : m_blackPieces)
    {
        piece->Render();
    }
}

void ChessBoard::Update(float deltaTime)
{
    Vec2 mousePos = Mouse::GetWorldPosition();
    Vec2 gridPos = WorldToGrid(mousePos);
    if (IsInBounds(gridPos) && Mouse::IsButtonPressed(GLFW_MOUSE_BUTTON_1))
    {
        OnMouseClick(gridPos);
    }
}

bool ChessBoard::IsValidPosition(const Vec2 &gridPos) const
{
    return IsInBounds(gridPos) && !IsOccupied(gridPos);
}

bool ChessBoard::IsOccupied(const Vec2 &gridPos) const
{
    for (const auto &piece : m_whitePieces)
    {
        if (piece->GetGridCell() == gridPos)
        {
            return true;
        }
    }
    for (const auto &piece : m_blackPieces)
    {
        if (piece->GetGridCell() == gridPos)
        {
            return true;
        }
    }
    return false;
}

void ChessBoard::AddPiece(ChessPiece *piece)
{
    if (piece->GetPieceColor() == ChessPieceColor::Black)
    {
        m_blackPieces.push_back(piece);
    }
    else
    {
        m_whitePieces.push_back(piece);
    }
}

void ChessBoard::OnMouseClick(const Vec2 &gridPos)
{
    // Check if a piece exists at the clicked cell
    Vec2 cell = GetCellFromGridPosition(gridPos);
    auto pieces = (m_currentPlayerColor == ChessPieceColor::White) ? m_whitePieces : m_blackPieces;
    // Check for piece selection
    for (const auto &piece : pieces)
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
    // If a piece is already selected, attempt to move it
    if (m_selectedPiece)
    {
        auto possibleMoves = m_selectedPiece->GetPossibleMoves();
        if (std::find(possibleMoves.begin(), possibleMoves.end(), cell) != possibleMoves.end())
        {
            // Move is valid
            m_selectedPiece->Deselect();
            m_selectedPiece->OnMove(cell);
            m_selectedPiece = nullptr;

            // Switch player turn
            m_currentPlayerColor = (m_currentPlayerColor == ChessPieceColor::White) ? ChessPieceColor::Black : ChessPieceColor::White;
        }
        else
        {
            // Invalid move, just deselect
            m_selectedPiece->Deselect();
            m_selectedPiece = nullptr;
        }
    }
}

ChessTile *ChessBoard::GetTile(const Vec2 &cell) const
{
    if (!IsInBounds(cell))
        return nullptr;
    int index = static_cast<int>(cell.y) * 8 + static_cast<int>(cell.x);
    if (index >= 0 && index < static_cast<int>(m_tiles.size()))
        return m_tiles[index];
    return nullptr;
}
