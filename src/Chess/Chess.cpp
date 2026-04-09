#include "Chess.h"
#include "Pieces/ChessPieces.h"
#include <iostream>

void Chess::Initialize()
{
    Vec2 position{0, 0};
    float cellSize = Renderer2D::GetCamera().GetWorldWidth() / ChessConstants::BOARD_SIZE;

    auto *scene = GetScene();
    m_board = scene->Instantiate<ChessBoard>(position, cellSize);

    // Initialize pawns
    for (int x = 0; x < ChessConstants::BOARD_SIZE; ++x)
    {
        m_board->AddPiece(scene->Instantiate<Pawn>(m_board, Vec2{x, ChessConstants::WHITE_PAWN_ROW}, PieceColor::White));
        m_board->AddPiece(scene->Instantiate<Pawn>(m_board, Vec2{x, ChessConstants::BLACK_PAWN_ROW}, PieceColor::Black));
    }

    // Initialize symmetric back-row pieces (two per type per color)
    for (int col : ChessConstants::ROOK_COLUMNS)
    {
        m_board->AddPiece(scene->Instantiate<Rook>(m_board, Vec2{col, ChessConstants::WHITE_BACK_ROW}, PieceColor::White));
        m_board->AddPiece(scene->Instantiate<Rook>(m_board, Vec2{col, ChessConstants::BLACK_BACK_ROW}, PieceColor::Black));
    }
    for (int col : ChessConstants::KNIGHT_COLUMNS)
    {
        m_board->AddPiece(scene->Instantiate<Knight>(m_board, Vec2{col, ChessConstants::WHITE_BACK_ROW}, PieceColor::White));
        m_board->AddPiece(scene->Instantiate<Knight>(m_board, Vec2{col, ChessConstants::BLACK_BACK_ROW}, PieceColor::Black));
    }
    for (int col : ChessConstants::BISHOP_COLUMNS)
    {
        m_board->AddPiece(scene->Instantiate<Bishop>(m_board, Vec2{col, ChessConstants::WHITE_BACK_ROW}, PieceColor::White));
        m_board->AddPiece(scene->Instantiate<Bishop>(m_board, Vec2{col, ChessConstants::BLACK_BACK_ROW}, PieceColor::Black));
    }

    // Initialize unique back-row pieces (one per color)
    m_board->AddPiece(scene->Instantiate<Queen>(m_board, Vec2{ChessConstants::QUEEN_COLUMN, ChessConstants::WHITE_BACK_ROW}, PieceColor::White));
    m_board->AddPiece(scene->Instantiate<Queen>(m_board, Vec2{ChessConstants::QUEEN_COLUMN, ChessConstants::BLACK_BACK_ROW}, PieceColor::Black));

    m_board->AddPiece(scene->Instantiate<King>(m_board, Vec2{ChessConstants::KING_COLUMN, ChessConstants::WHITE_BACK_ROW}, PieceColor::White));
    m_board->AddPiece(scene->Instantiate<King>(m_board, Vec2{ChessConstants::KING_COLUMN, ChessConstants::BLACK_BACK_ROW}, PieceColor::Black));

    Mouse::SetCursorVisibility(false);
}

void Chess::Update(float deltaTime)
{
    m_inputManager.ProcessInput();
    if (m_inputManager.IsExitRequested())
    {
        Close();
    }
    m_board->Update(deltaTime);
    if (m_board->IsGameOver())
    {
        std::cout << "Game Over! " << ((m_board->GetCurrentPlayerColor() == PieceColor::White) ? "White" : "Black") << " wins!" << std::endl;
        Close();
    }
}

void Chess::Render() const
{
    m_board->Render();
    Vec2 mousePosition = Mouse::GetWorldPosition();
    Renderer2D::DrawCircle(mousePosition, ChessConstants::CURSOR_RADIUS, Color::Red);
}

WindowConfig Chess::GetWindowConfig() const
{
    WindowConfig config;
    config.title = "Chess";
    config.width = ChessConstants::WINDOW_SIZE;
    config.height = ChessConstants::WINDOW_SIZE;
    return config;
}
