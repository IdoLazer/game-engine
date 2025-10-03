#include "Chess.h"
#include "Pieces/Pawn.h"

void Chess::Initialize()
{
    Vec2 position{0, 0};
    float cellSize = Renderer2D::GetCamera().GetWorldWidth() / 8.0f;
    Vec2 cellCount{8, 8};

    m_board = new ChessBoard(position, cellSize);
    m_board->Initialize();

    // Initialize pieces
    // Initialize pawns
    for (int x = 0; x < 8; ++x)
    {
        auto whitePawn = new Pawn(m_board, Vec2{x, 1}, ChessPieceColor::White);
        whitePawn->Initialize();
        m_board->AddPiece(whitePawn);
        auto blackPawn = new Pawn(m_board, Vec2{x, 6}, ChessPieceColor::Black);
        blackPawn->Initialize();
        m_board->AddPiece(blackPawn);
    }

    m_inputManager = new InputManager();

    Mouse::SetCursorVisibility(false);
}

void Chess::Update(float deltaTime)
{
    m_inputManager->ProcessInput();
    if (m_inputManager->IsExitRequested())
    {
        Close();
    }
    m_board->Update(deltaTime);
}

void Chess::Render()
{
    m_board->Render();
    Vec2 mousePosition = Mouse::GetWorldPosition();
    Renderer2D::DrawCircle(mousePosition, 0.1f, Color::Red);
}

void Chess::Shutdown()
{
    delete m_board;
    delete m_inputManager;
}

WindowConfig Chess::GetWindowConfig() const
{
    WindowConfig config;
    config.title = "Chess";
    config.width = 600;
    config.height = 600;
    return config;
}
