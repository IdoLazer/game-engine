#pragma once

#include <Engine.h>
#include <Core/EntryPoint.h>
#include <vector>

#include "InputManager.h"
#include "ChessBoard.h"
#include "ChessTypes.h"

class ChessPiece;
class King;
class Rook;

class Chess : public Engine::Application
{
public:
    // --- Constructors & Destructors ---
    Chess() = default;

    // --- Game Interface ---
    void Initialize() override;
    void Update(float deltaTime) override;
    void Render() const override;
    void Shutdown() override;

    Engine::WindowConfig GetWindowConfig() const override;

private:
    // --- Game Logic ---
    void OnMouseClick(const Engine::Vec2 &cell);
    void AddPiece(ChessPiece *piece);
    void ToggleHighlight(ChessPiece *piece, bool highlight);

    // --- Castling ---
    std::vector<Engine::Vec2> GetCastlingMoves(King *king) const;
    bool IsCastlingMove(const Engine::Vec2 &cell) const;
    void PerformCastle(King *king, const Engine::Vec2 &kingDest);

    // --- Fields ---
    Engine::Grid m_grid;
    ChessBoard *m_board{nullptr};
    std::vector<ChessPiece *> m_whitePieces;
    std::vector<ChessPiece *> m_blackPieces;
    ChessPiece *m_selectedPiece{nullptr};
    PieceColor m_currentPlayerColor{PieceColor::White};
    bool m_gameOver{false};
    InputManager m_inputManager;
};