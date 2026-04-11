#pragma once

#include <Engine.h>
#include <Core/EntryPoint.h>
#include <vector>

#include "InputManager.h"
#include "ChessBoard.h"
#include "ChessTypes.h"

class ChessPiece;
class King;

class Chess : public Engine::Application
{
private:
    // World
    Engine::Grid m_grid;

    // Rendering
    ChessBoard *m_board{nullptr};

    // Pieces
    std::vector<ChessPiece *> m_whitePieces;
    std::vector<ChessPiece *> m_blackPieces;

    // Game state
    ChessPiece *m_selectedPiece{nullptr};
    PieceColor m_currentPlayerColor{PieceColor::White};
    bool m_gameOver{false};

    // Input
    InputManager m_inputManager;

    void AddPiece(ChessPiece *piece);
    void OnMouseClick(const Engine::Vec2 &cell);
    void ToggleHighlight(ChessPiece *piece, bool highlight);

public:
    Chess();
    void Initialize() override;
    void Update(float deltaTime) override;
    void Render() const override;
    void Shutdown() override {};

    Engine::WindowConfig GetWindowConfig() const override;
};

namespace Engine
{
    Application *CreateApplication()
    {
        return new Chess();
    }
}