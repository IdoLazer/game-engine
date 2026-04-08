#include "Chess.h"
#include "Pieces/ChessPieces.h"
#include <iostream>

void Chess::Initialize()
{
    Vec2 position{0, 0};
    float cellSize = Renderer2D::GetCamera().GetWorldWidth() / 8.0f;

    auto *scene = GetScene();
    m_board = scene->Instantiate<ChessBoard>(position, cellSize);

    // Initialize pawns
    for (int x = 0; x < 8; ++x)
    {
        m_board->AddPiece(scene->Instantiate<Pawn>(m_board, Vec2{x, 1}, ChessPieceColor::White));
        m_board->AddPiece(scene->Instantiate<Pawn>(m_board, Vec2{x, 6}, ChessPieceColor::Black));
    }

    // Initialize rooks
    m_board->AddPiece(scene->Instantiate<Rook>(m_board, Vec2{0, 0}, ChessPieceColor::White));
    m_board->AddPiece(scene->Instantiate<Rook>(m_board, Vec2{7, 0}, ChessPieceColor::White));
    m_board->AddPiece(scene->Instantiate<Rook>(m_board, Vec2{0, 7}, ChessPieceColor::Black));
    m_board->AddPiece(scene->Instantiate<Rook>(m_board, Vec2{7, 7}, ChessPieceColor::Black));

    // Initialize knights
    m_board->AddPiece(scene->Instantiate<Knight>(m_board, Vec2{1, 0}, ChessPieceColor::White));
    m_board->AddPiece(scene->Instantiate<Knight>(m_board, Vec2{6, 0}, ChessPieceColor::White));
    m_board->AddPiece(scene->Instantiate<Knight>(m_board, Vec2{1, 7}, ChessPieceColor::Black));
    m_board->AddPiece(scene->Instantiate<Knight>(m_board, Vec2{6, 7}, ChessPieceColor::Black));

    // Initialize bishops
    m_board->AddPiece(scene->Instantiate<Bishop>(m_board, Vec2{2, 0}, ChessPieceColor::White));
    m_board->AddPiece(scene->Instantiate<Bishop>(m_board, Vec2{5, 0}, ChessPieceColor::White));
    m_board->AddPiece(scene->Instantiate<Bishop>(m_board, Vec2{2, 7}, ChessPieceColor::Black));
    m_board->AddPiece(scene->Instantiate<Bishop>(m_board, Vec2{5, 7}, ChessPieceColor::Black));

    // Initialize queens
    m_board->AddPiece(scene->Instantiate<Queen>(m_board, Vec2{3, 0}, ChessPieceColor::White));
    m_board->AddPiece(scene->Instantiate<Queen>(m_board, Vec2{3, 7}, ChessPieceColor::Black));

    // Initialize kings
    m_board->AddPiece(scene->Instantiate<King>(m_board, Vec2{4, 0}, ChessPieceColor::White));
    m_board->AddPiece(scene->Instantiate<King>(m_board, Vec2{4, 7}, ChessPieceColor::Black));

    // m_inputManager = std::make_unique<InputManager>();

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
        std::cout << "Game Over! " << ((m_board->GetCurrentPlayerColor() == ChessPieceColor::White) ? "White" : "Black") << " wins!" << std::endl;
        Close();
    }
}

void Chess::Render() const
{
    m_board->Render();
    Vec2 mousePosition = Mouse::GetWorldPosition();
    Renderer2D::DrawCircle(mousePosition, 0.1f, Color::Red);
}

WindowConfig Chess::GetWindowConfig() const
{
    WindowConfig config;
    config.title = "Chess";
    config.width = 600;
    config.height = 600;
    return config;
}
