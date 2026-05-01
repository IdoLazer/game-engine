#pragma once
#include <vector>
#include <string>
#include <Engine.h>
#include "../ChessConstants.h"
#include "../ChessTypes.h"

class ChessPiece : public Engine::GridEntity
{
    DECLARE_TYPE(ChessPiece, GridEntity)

// --- Fields ---
protected:
    PieceColor m_pieceColor{};
    Engine::Sprite m_sprite;

// --- Constructors & Destructors ---
public:
    ChessPiece() = default;

// --- Lifecycle ---
public:
    virtual void Initialize() override;
    virtual void Render() const override;

// --- Accessors ---
public:
    PieceColor GetPieceColor() const;

// --- Subclass Interface ---
public:
    virtual std::vector<Engine::Vec2> GetPossibleMoves() const = 0;

    // Returns the piece name used to build the texture path (e.g. "Pawn", "Rook").
    // Override in each subclass. Returns empty string if no texture exists yet.
    virtual std::string GetPieceName() const { return ""; }

// --- Internal ---
protected:
    std::vector<Engine::Vec2> GetSlidingMoves(const std::vector<Engine::Vec2> &directions) const;
    std::vector<Engine::Vec2> GetSteppingMoves(const std::vector<Engine::Vec2> &directions) const;
    bool IsCellEmpty(const Engine::Vec2 &pos) const;
    bool IsCellValidAndEmpty(const Engine::Vec2 &pos) const;
    ChessPiece *GetPieceAt(const Engine::Vec2 &pos) const;
};