#include "Chess.h"
#include "Pieces/ChessPieces.h"
#include <iostream>

using namespace Engine;

// --- Game Interface ---

void Chess::Initialize()
{
    // Calculate cell size to fit the board in the viewport
    float cellSize = Renderer2D::GetCamera().GetWorldWidth() / ChessConstants::BOARD_SIZE;

    // Construct Grid
    m_grid = Grid(cellSize, Vec2{ChessConstants::BOARD_SIZE, ChessConstants::BOARD_SIZE});

    auto *scene = GetScene();

    // Create the tile map for rendering
    m_board = scene->Instantiate<ChessBoard>();
    m_board->SetCoordSystem(&m_grid.GetCoordinateSystem());
    m_board->Initialize();

    // Initialize all pieces from data table — properties flow through the
    // property system, then we wire up the runtime Grid* pointer manually.
    for (const auto &pieceData : ChessConstants::CHESS_PIECES_DATA)
    {
        auto *piece = static_cast<ChessPiece *>(scene->Instantiate(pieceData));
        piece->SetGrid(&m_grid);
        piece->Initialize();
        AddPiece(piece);
    }

    Mouse::SetCursorVisibility(false);
}

void Chess::Update(float deltaTime)
{
    m_inputManager.ProcessInput();
    if (m_inputManager.IsExitRequested())
    {
        Close();
        return;
    }

    if (m_gameOver)
        return;

    // Handle mouse input
    Vec2 mousePos = Mouse::GetWorldPosition();
    Vec2 gridPos = m_grid.WorldToGrid(mousePos);
    if (m_grid.IsInBounds(gridPos) && Mouse::IsButtonPressed(MouseButton::Left))
    {
        Vec2 cell = m_grid.GetCellFromGridPosition(gridPos);
        OnMouseClick(cell);
    }
}

void Chess::Render() const
{
    // Render tiles
    m_board->Render();

    // Render pieces
    for (const auto *piece : m_whitePieces)
        piece->Render();
    for (const auto *piece : m_blackPieces)
        piece->Render();

    // Render cursor
    Vec2 mousePosition = Mouse::GetWorldPosition();
    Renderer2D::DrawCircle(mousePosition, ChessConstants::CURSOR_RADIUS, Color::Red);
}

WindowConfig Chess::GetWindowConfig() const
{
    WindowConfig config;
    config.title = ChessConstants::WINDOW_TITLE;
    config.width = ChessConstants::WINDOW_SIZE;
    config.height = ChessConstants::WINDOW_SIZE;
    return config;
}

void Chess::Shutdown() {}

// --- Game Logic ---

void Chess::AddPiece(ChessPiece *piece)
{
    if (piece->GetPieceColor() == PieceColor::Black)
        m_blackPieces.push_back(piece);
    else
        m_whitePieces.push_back(piece);
}

void Chess::OnMouseClick(const Vec2 &cell)
{
    // Use the Grid spatial index to find what's at the clicked cell
    ChessPiece *clickedPiece = m_grid.GetFirstEntityAt<ChessPiece>(cell);

    // Check for piece selection (clicked a piece of the current player's color)
    if (clickedPiece && clickedPiece->GetPieceColor() == m_currentPlayerColor)
    {
        if (m_selectedPiece && clickedPiece != m_selectedPiece)
            // Clear highlights from previously selected piece
            ToggleHighlight(m_selectedPiece, false);

        m_selectedPiece = clickedPiece;

        // Highlight the selected piece and its possible moves
        ToggleHighlight(m_selectedPiece, true);
        return;
    }

    // If a piece is already selected, attempt to move it
    if (m_selectedPiece)
    {
        auto possibleMoves = m_selectedPiece->GetPossibleMoves();
        bool isNormalMove = std::find(possibleMoves.begin(), possibleMoves.end(), cell) != possibleMoves.end();
        bool isCastle = !isNormalMove && IsCastlingMove(cell);

        if (isNormalMove || isCastle)
        {
            // Clear highlights from the selected piece
            ToggleHighlight(m_selectedPiece, false);

            if (isCastle)
            {
                // Castling — move both king and rook
                PerformCastle(dynamic_cast<King *>(m_selectedPiece), cell);
            }
            else
            {
                // Normal move — capture first, then move
                ChessPiece *captured = m_grid.GetFirstEntityAt<ChessPiece>(cell);
                if (captured)
                {
                    if (dynamic_cast<King *>(captured))
                    {
                        m_gameOver = true;
                        std::cout << "Game Over! "
                                  << ((m_currentPlayerColor == PieceColor::White) ? "White" : "Black")
                                  << " wins!" << std::endl;
                        Close();
                        return;
                    }
                    auto &opponentPieces = (m_currentPlayerColor == PieceColor::White) ? m_blackPieces : m_whitePieces;
                    std::erase(opponentPieces, captured);
                    GetScene()->Destroy(captured);
                }

                m_selectedPiece->SetGridPosition(cell);
            }

            m_selectedPiece = nullptr;

            // Switch player turn
            m_currentPlayerColor = (m_currentPlayerColor == PieceColor::White) ? PieceColor::Black : PieceColor::White;
        }
        else
        {
            // Invalid move, just deselect
            ToggleHighlight(m_selectedPiece, false);
            m_selectedPiece = nullptr;
        }
    }
}

void Chess::ToggleHighlight(ChessPiece *piece, bool highlight)
{
    m_board->ToggleHighlight(piece->GetGridCell(), highlight);
    for (auto &cell : piece->GetPossibleMoves())
    {
        m_board->ToggleHighlight(cell, highlight);
    }

    // Also highlight castling moves if this is a king
    King *king = dynamic_cast<King *>(piece);
    if (king)
    {
        for (auto &cell : GetCastlingMoves(king))
        {
            m_board->ToggleHighlight(cell, highlight);
        }
    }
}

// --- Castling ---

std::vector<Vec2> Chess::GetCastlingMoves(King *king) const
{
    std::vector<Vec2> moves;
    if (king->HasMoved())
        return moves;

    float row = king->GetGridCell().y;

    // Check kingside castle (rook at column 7)
    Vec2 kingsideRookCell{static_cast<float>(ChessConstants::KINGSIDE_ROOK_COLUMN), row};
    Rook *kingsideRook = m_grid.GetFirstEntityAt<Rook>(kingsideRookCell);
    if (kingsideRook && !kingsideRook->HasMoved() && kingsideRook->GetPieceColor() == king->GetPieceColor())
    {
        // Check that cells between king and rook are empty (columns 5 and 6)
        bool pathClear = true;
        for (int col = ChessConstants::KING_COLUMN + 1; col < ChessConstants::KINGSIDE_ROOK_COLUMN; ++col)
        {
            if (m_grid.GetFirstEntityAt<ChessPiece>(Vec2{static_cast<float>(col), row}))
            {
                pathClear = false;
                break;
            }
        }
        if (pathClear)
            moves.push_back(Vec2{static_cast<float>(ChessConstants::KING_CASTLE_KINGSIDE_COLUMN), row});
    }

    // Check queenside castle (rook at column 0)
    Vec2 queensideRookCell{static_cast<float>(ChessConstants::QUEENSIDE_ROOK_COLUMN), row};
    Rook *queensideRook = m_grid.GetFirstEntityAt<Rook>(queensideRookCell);
    if (queensideRook && !queensideRook->HasMoved() && queensideRook->GetPieceColor() == king->GetPieceColor())
    {
        // Check that cells between rook and king are empty (columns 1, 2, 3)
        bool pathClear = true;
        for (int col = ChessConstants::QUEENSIDE_ROOK_COLUMN + 1; col < ChessConstants::KING_COLUMN; ++col)
        {
            if (m_grid.GetFirstEntityAt<ChessPiece>(Vec2{static_cast<float>(col), row}))
            {
                pathClear = false;
                break;
            }
        }
        if (pathClear)
            moves.push_back(Vec2{static_cast<float>(ChessConstants::KING_CASTLE_QUEENSIDE_COLUMN), row});
    }

    return moves;
}

bool Chess::IsCastlingMove(const Vec2 &cell) const
{
    King *king = dynamic_cast<King *>(m_selectedPiece);
    if (!king)
        return false;

    auto castlingMoves = GetCastlingMoves(king);
    return std::find(castlingMoves.begin(), castlingMoves.end(), cell) != castlingMoves.end();
}

void Chess::PerformCastle(King *king, const Vec2 &kingDest)
{
    float row = king->GetGridCell().y;

    // Determine which side we're castling
    bool isKingside = (kingDest.x == ChessConstants::KING_CASTLE_KINGSIDE_COLUMN);

    // Find the rook
    float rookCol = isKingside ? ChessConstants::KINGSIDE_ROOK_COLUMN : ChessConstants::QUEENSIDE_ROOK_COLUMN;
    float rookDest = isKingside ? ChessConstants::ROOK_CASTLE_KINGSIDE_COLUMN : ChessConstants::ROOK_CASTLE_QUEENSIDE_COLUMN;

    Rook *rook = m_grid.GetFirstEntityAt<Rook>(Vec2{rookCol, row});

    // Move both pieces
    king->SetGridPosition(kingDest);
    rook->SetGridPosition(Vec2{rookDest, row});
}

// Engine Entry Point Factory Function
namespace Engine
{
    Application *CreateApplication()
    {
        return new Chess();
    }
}
