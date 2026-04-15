#pragma once
#include <vector>
#include <Engine.h>
#include "../ChessConstants.h"
#include "../ChessTypes.h"

class ChessPiece : public Engine::GridEntity
{
    DECLARE_TYPE(ChessPiece, GridEntity)

// --- Fields ---
protected:
    PieceColor m_pieceColor{};

// --- Constructors & Destructors ---
public:
    ChessPiece() = default;

// --- Lifecycle ---
public:
    virtual void Initialize() override;

// --- Accessors ---
public:
    PieceColor GetPieceColor() const;

// --- Subclass Interface ---
public:
    virtual std::vector<Engine::Vec2> GetPossibleMoves() const = 0;

// --- Internal ---
protected:
    std::vector<Engine::Vec2> GetSlidingMoves(const std::vector<Engine::Vec2> &directions) const;
    std::vector<Engine::Vec2> GetSteppingMoves(const std::vector<Engine::Vec2> &directions) const;
    bool IsCellEmpty(const Engine::Vec2 &pos) const;
    bool IsCellValidAndEmpty(const Engine::Vec2 &pos) const;
    ChessPiece *GetPieceAt(const Engine::Vec2 &pos) const;
};