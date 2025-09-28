#include "Chess.h"

void Chess::Initialize()
{
    Vec2 position{0, 0};
    float cellSize = Renderer2D::GetCamera().GetWorldWidth() / 8.0f;
    Vec2 cellCount{8, 8};

    m_board = new ChessBoard(position, cellSize);
    m_board->Initialize();

    m_inputManager = new InputManager();
}

void Chess::Update(float deltaTime)
{
    m_inputManager->ProcessInput();
    if (m_inputManager->IsExitRequested())
    {
        Close();
    }
}

void Chess::Render()
{
    m_board->Render();
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
    config.width = 1200;
    config.height = 1200;
    return config;
}
