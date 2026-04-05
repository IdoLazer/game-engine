#include "Chess.h"
#include "Pieces/ChessPieces.h"

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
    // Initialize rooks
    auto whiteRook1 = new Rook(m_board, Vec2{0, 0}, ChessPieceColor::White);
    whiteRook1->Initialize();
    m_board->AddPiece(whiteRook1);
    auto whiteRook2 = new Rook(m_board, Vec2{7, 0}, ChessPieceColor::White);
    whiteRook2->Initialize();
    m_board->AddPiece(whiteRook2);
    auto blackRook1 = new Rook(m_board, Vec2{0, 7}, ChessPieceColor::Black);
    blackRook1->Initialize();
    m_board->AddPiece(blackRook1);
    auto blackRook2 = new Rook(m_board, Vec2{7, 7}, ChessPieceColor::Black);
    blackRook2->Initialize();
    m_board->AddPiece(blackRook2);

    // Initialize knights
    auto whiteKnight1 = new Knight(m_board, Vec2{1, 0}, ChessPieceColor::White);
    whiteKnight1->Initialize();
    m_board->AddPiece(whiteKnight1);
    auto whiteKnight2 = new Knight(m_board, Vec2{6, 0}, ChessPieceColor::White);
    whiteKnight2->Initialize();
    m_board->AddPiece(whiteKnight2);
    auto blackKnight1 = new Knight(m_board, Vec2{1, 7}, ChessPieceColor::Black);
    blackKnight1->Initialize();
    m_board->AddPiece(blackKnight1);
    auto blackKnight2 = new Knight(m_board, Vec2{6, 7}, ChessPieceColor::Black);
    blackKnight2->Initialize();
    m_board->AddPiece(blackKnight2);

    // Initialize bishops
    auto whiteBishop1 = new Bishop(m_board, Vec2{2, 0}, ChessPieceColor::White);
    whiteBishop1->Initialize();
    m_board->AddPiece(whiteBishop1);
    auto whiteBishop2 = new Bishop(m_board, Vec2{5, 0}, ChessPieceColor::White);
    whiteBishop2->Initialize();
    m_board->AddPiece(whiteBishop2);
    auto blackBishop1 = new Bishop(m_board, Vec2{2, 7}, ChessPieceColor::Black);
    blackBishop1->Initialize();
    m_board->AddPiece(blackBishop1);
    auto blackBishop2 = new Bishop(m_board, Vec2{5, 7}, ChessPieceColor::Black);
    blackBishop2->Initialize();
    m_board->AddPiece(blackBishop2);

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

void Chess::Render() const
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
