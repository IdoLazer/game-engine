#pragma once

#include <Engine.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <any>
#include "ChessTypes.h"

namespace ChessConstants
{
    // Board
    static constexpr int BOARD_SIZE = 8;

    // Window
    const std::string WINDOW_TITLE = "Chess";
    static constexpr int WINDOW_SIZE = 600;

    // Tile colors
    const Engine::Color TILE_COLOR_DARK{139.0f / 255.0f, 0.0f, 0.0f, 1.0f};
    const Engine::Color TILE_COLOR_LIGHT{198.0f / 255.0f, 218.0f / 255.0f, 167.0f / 255.0f, 1.0f};

    // Tile highlight
    const Engine::Color TILE_HIGHLIGHT_TINT{0.1f, 0.1f, 0.0f, 1.0f};
    const Engine::Color TILE_HIGHLIGHT_OUTLINE{1.0f, 1.0f, 0.0f, 0.8f};

    // Piece entity colors (visual fill)
    const Engine::Color PIECE_COLOR_WHITE = Engine::Color::White;
    const Engine::Color PIECE_COLOR_BLACK = Engine::Color::Black;

    // Piece sizes (grid-relative)
    const Engine::Vec2 PAWN_SIZE{1.0f, 1.0f};
    const Engine::Vec2 ROOK_SIZE{0.4f, 0.6f};
    const Engine::Vec2 KNIGHT_SIZE{0.3f, 0.6f};
    const Engine::Vec2 BISHOP_SIZE{1.0f, 1.0f};
    const Engine::Vec2 QUEEN_SIZE{1.0f, 1.0f};
    const Engine::Vec2 KING_SIZE{1.0f, 1.0f};

    // Directions — cardinal
    const Engine::Vec2 DIRECTION_N{0, 1};
    const Engine::Vec2 DIRECTION_S{0, -1};
    const Engine::Vec2 DIRECTION_E{1, 0};
    const Engine::Vec2 DIRECTION_W{-1, 0};

    // Directions — diagonal
    const Engine::Vec2 DIRECTION_NE{1, 1};
    const Engine::Vec2 DIRECTION_NW{-1, 1};
    const Engine::Vec2 DIRECTION_SE{1, -1};
    const Engine::Vec2 DIRECTION_SW{-1, -1};

    // Cursor
    static constexpr float CURSOR_RADIUS = 0.1f;

    // Castling
    static constexpr int KING_COLUMN = 4;
    static constexpr int KINGSIDE_ROOK_COLUMN = 7;
    static constexpr int QUEENSIDE_ROOK_COLUMN = 0;
    static constexpr int KING_CASTLE_KINGSIDE_COLUMN = 6;
    static constexpr int KING_CASTLE_QUEENSIDE_COLUMN = 2;
    static constexpr int ROOK_CASTLE_KINGSIDE_COLUMN = 5;
    static constexpr int ROOK_CASTLE_QUEENSIDE_COLUMN = 3;

    // --- Initial board layout (flat, data-file-like) ---

    using EntityInfo = Engine::Scene::EntityInfo;
    using PropertyMap = Engine::PropertyMap;

    static const std::vector<EntityInfo> CHESS_PIECES_DATA = {
        // White Pawns
        {"Pawn", PropertyMap{{"GridPosition", Engine::Vec2(0, 1)}, {"GridSize", PAWN_SIZE}, {"PieceColor", PieceColor::White}}},
        {"Pawn", PropertyMap{{"GridPosition", Engine::Vec2(1, 1)}, {"GridSize", PAWN_SIZE}, {"PieceColor", PieceColor::White}}},
        {"Pawn", PropertyMap{{"GridPosition", Engine::Vec2(2, 1)}, {"GridSize", PAWN_SIZE}, {"PieceColor", PieceColor::White}}},
        {"Pawn", PropertyMap{{"GridPosition", Engine::Vec2(3, 1)}, {"GridSize", PAWN_SIZE}, {"PieceColor", PieceColor::White}}},
        {"Pawn", PropertyMap{{"GridPosition", Engine::Vec2(4, 1)}, {"GridSize", PAWN_SIZE}, {"PieceColor", PieceColor::White}}},
        {"Pawn", PropertyMap{{"GridPosition", Engine::Vec2(5, 1)}, {"GridSize", PAWN_SIZE}, {"PieceColor", PieceColor::White}}},
        {"Pawn", PropertyMap{{"GridPosition", Engine::Vec2(6, 1)}, {"GridSize", PAWN_SIZE}, {"PieceColor", PieceColor::White}}},
        {"Pawn", PropertyMap{{"GridPosition", Engine::Vec2(7, 1)}, {"GridSize", PAWN_SIZE}, {"PieceColor", PieceColor::White}}},

        // Black Pawns
        {"Pawn", PropertyMap{{"GridPosition", Engine::Vec2(0, 6)}, {"GridSize", PAWN_SIZE}, {"PieceColor", PieceColor::Black}}},
        {"Pawn", PropertyMap{{"GridPosition", Engine::Vec2(1, 6)}, {"GridSize", PAWN_SIZE}, {"PieceColor", PieceColor::Black}}},
        {"Pawn", PropertyMap{{"GridPosition", Engine::Vec2(2, 6)}, {"GridSize", PAWN_SIZE}, {"PieceColor", PieceColor::Black}}},
        {"Pawn", PropertyMap{{"GridPosition", Engine::Vec2(3, 6)}, {"GridSize", PAWN_SIZE}, {"PieceColor", PieceColor::Black}}},
        {"Pawn", PropertyMap{{"GridPosition", Engine::Vec2(4, 6)}, {"GridSize", PAWN_SIZE}, {"PieceColor", PieceColor::Black}}},
        {"Pawn", PropertyMap{{"GridPosition", Engine::Vec2(5, 6)}, {"GridSize", PAWN_SIZE}, {"PieceColor", PieceColor::Black}}},
        {"Pawn", PropertyMap{{"GridPosition", Engine::Vec2(6, 6)}, {"GridSize", PAWN_SIZE}, {"PieceColor", PieceColor::Black}}},
        {"Pawn", PropertyMap{{"GridPosition", Engine::Vec2(7, 6)}, {"GridSize", PAWN_SIZE}, {"PieceColor", PieceColor::Black}}},

        // White Back Row
        {"Rook",   PropertyMap{{"GridPosition", Engine::Vec2(0, 0)}, {"GridSize", ROOK_SIZE},   {"PieceColor", PieceColor::White}}},
        {"Knight", PropertyMap{{"GridPosition", Engine::Vec2(1, 0)}, {"GridSize", KNIGHT_SIZE}, {"PieceColor", PieceColor::White}}},
        {"Bishop", PropertyMap{{"GridPosition", Engine::Vec2(2, 0)}, {"GridSize", BISHOP_SIZE}, {"PieceColor", PieceColor::White}}},
        {"Queen",  PropertyMap{{"GridPosition", Engine::Vec2(3, 0)}, {"GridSize", QUEEN_SIZE},  {"PieceColor", PieceColor::White}}},
        {"King",   PropertyMap{{"GridPosition", Engine::Vec2(4, 0)}, {"GridSize", KING_SIZE},   {"PieceColor", PieceColor::White}}},
        {"Bishop", PropertyMap{{"GridPosition", Engine::Vec2(5, 0)}, {"GridSize", BISHOP_SIZE}, {"PieceColor", PieceColor::White}}},
        {"Knight", PropertyMap{{"GridPosition", Engine::Vec2(6, 0)}, {"GridSize", KNIGHT_SIZE}, {"PieceColor", PieceColor::White}}},
        {"Rook",   PropertyMap{{"GridPosition", Engine::Vec2(7, 0)}, {"GridSize", ROOK_SIZE},   {"PieceColor", PieceColor::White}}},

        // Black Back Row
        {"Rook",   PropertyMap{{"GridPosition", Engine::Vec2(0, 7)}, {"GridSize", ROOK_SIZE},   {"PieceColor", PieceColor::Black}}},
        {"Knight", PropertyMap{{"GridPosition", Engine::Vec2(1, 7)}, {"GridSize", KNIGHT_SIZE}, {"PieceColor", PieceColor::Black}}},
        {"Bishop", PropertyMap{{"GridPosition", Engine::Vec2(2, 7)}, {"GridSize", BISHOP_SIZE}, {"PieceColor", PieceColor::Black}}},
        {"Queen",  PropertyMap{{"GridPosition", Engine::Vec2(3, 7)}, {"GridSize", QUEEN_SIZE},  {"PieceColor", PieceColor::Black}}},
        {"King",   PropertyMap{{"GridPosition", Engine::Vec2(4, 7)}, {"GridSize", KING_SIZE},   {"PieceColor", PieceColor::Black}}},
        {"Bishop", PropertyMap{{"GridPosition", Engine::Vec2(5, 7)}, {"GridSize", BISHOP_SIZE}, {"PieceColor", PieceColor::Black}}},
        {"Knight", PropertyMap{{"GridPosition", Engine::Vec2(6, 7)}, {"GridSize", KNIGHT_SIZE}, {"PieceColor", PieceColor::Black}}},
        {"Rook",   PropertyMap{{"GridPosition", Engine::Vec2(7, 7)}, {"GridSize", ROOK_SIZE},   {"PieceColor", PieceColor::Black}}},
    };
}