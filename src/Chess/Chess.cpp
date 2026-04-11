#include "Chess.h"
#include "Pieces/ChessPieces.h"
#include <iostream>

using namespace Engine;

Chess::Chess()
    : m_grid()
{
}

void Chess::Initialize()
{
    // Calculate cell size to fit the board in the viewport
    float cellSize = Renderer2D::GetCamera().GetWorldWidth() / ChessConstants::BOARD_SIZE;

    // Construct Grid
    m_grid = Grid(cellSize, Vec2{ChessConstants::BOARD_SIZE, ChessConstants::BOARD_SIZE});

    auto *scene = GetScene();

    // Create the tile map for rendering
    m_board = scene->Instantiate<ChessBoard>(&m_grid.GetCoordinateSystem());

    // Initialize pawns
    for (int x = 0; x < ChessConstants::BOARD_SIZE; ++x)
    {
        AddPiece(scene->Instantiate<Pawn>(&m_grid, Vec2{x, ChessConstants::WHITE_PAWN_ROW}, PieceColor::White));
        AddPiece(scene->Instantiate<Pawn>(&m_grid, Vec2{x, ChessConstants::BLACK_PAWN_ROW}, PieceColor::Black));
    }

    // Initialize symmetric back-row pieces
    for (int col : ChessConstants::ROOK_COLUMNS)
    {
        AddPiece(scene->Instantiate<Rook>(&m_grid, Vec2{col, ChessConstants::WHITE_BACK_ROW}, PieceColor::White));
        AddPiece(scene->Instantiate<Rook>(&m_grid, Vec2{col, ChessConstants::BLACK_BACK_ROW}, PieceColor::Black));
    }
    for (int col : ChessConstants::KNIGHT_COLUMNS)
    {
        AddPiece(scene->Instantiate<Knight>(&m_grid, Vec2{col, ChessConstants::WHITE_BACK_ROW}, PieceColor::White));
        AddPiece(scene->Instantiate<Knight>(&m_grid, Vec2{col, ChessConstants::BLACK_BACK_ROW}, PieceColor::Black));
    }
    for (int col : ChessConstants::BISHOP_COLUMNS)
    {
        AddPiece(scene->Instantiate<Bishop>(&m_grid, Vec2{col, ChessConstants::WHITE_BACK_ROW}, PieceColor::White));
        AddPiece(scene->Instantiate<Bishop>(&m_grid, Vec2{col, ChessConstants::BLACK_BACK_ROW}, PieceColor::Black));
    }

    // Initialize unique back-row pieces
    AddPiece(scene->Instantiate<Queen>(&m_grid, Vec2{ChessConstants::QUEEN_COLUMN, ChessConstants::WHITE_BACK_ROW}, PieceColor::White));
    AddPiece(scene->Instantiate<Queen>(&m_grid, Vec2{ChessConstants::QUEEN_COLUMN, ChessConstants::BLACK_BACK_ROW}, PieceColor::Black));

    AddPiece(scene->Instantiate<King>(&m_grid, Vec2{ChessConstants::KING_COLUMN, ChessConstants::WHITE_BACK_ROW}, PieceColor::White));
    AddPiece(scene->Instantiate<King>(&m_grid, Vec2{ChessConstants::KING_COLUMN, ChessConstants::BLACK_BACK_ROW}, PieceColor::Black));

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
        if (std::find(possibleMoves.begin(), possibleMoves.end(), cell) != possibleMoves.end())
        {
            // Clear highlights from the selected piece
            ToggleHighlight(m_selectedPiece, false);

            // If there's an opponent piece at the target cell, capture it
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
                opponentPieces.erase(std::remove(opponentPieces.begin(), opponentPieces.end(), captured), opponentPieces.end());
                GetScene()->Destroy(captured);
            }

            // Move into the (now-empty) cell
            m_selectedPiece->SetGridPosition(cell);
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
}
