#pragma once

#include <Engine.h>

using namespace Engine;

namespace ChessConstants
{
    // Board
    static constexpr int BOARD_SIZE = 8;

    // Window
    static constexpr int WINDOW_SIZE = 600;

    // Tile colors
    const Color TILE_COLOR_DARK{139.0f / 255.0f, 0.0f, 0.0f, 1.0f};
    const Color TILE_COLOR_LIGHT{198.0f / 255.0f, 218.0f / 255.0f, 167.0f / 255.0f, 1.0f};

    // Tile highlight
    const Color TILE_HIGHLIGHT_TINT{0.1f, 0.1f, 0.0f, 1.0f};
    const Color TILE_HIGHLIGHT_OUTLINE{1.0f, 1.0f, 0.0f, 0.8f};

    // Piece entity colors (visual fill)
    const Color PIECE_COLOR_WHITE = Color::White;
    const Color PIECE_COLOR_BLACK = Color::Black;

    // Piece sizes (grid-relative)
    const Vec2 PAWN_SIZE{1.0f, 1.0f};
    const Vec2 ROOK_SIZE{0.4f, 0.6f};
    const Vec2 KNIGHT_SIZE{0.3f, 0.6f};
    const Vec2 BISHOP_SIZE{1.0f, 1.0f};
    const Vec2 QUEEN_SIZE{1.0f, 1.0f};
    const Vec2 KING_SIZE{1.0f, 1.0f};

    // Directions — cardinal
    const Vec2 DIRECTION_N{0, 1};
    const Vec2 DIRECTION_S{0, -1};
    const Vec2 DIRECTION_E{1, 0};
    const Vec2 DIRECTION_W{-1, 0};

    // Directions — diagonal
    const Vec2 DIRECTION_NE{1, 1};
    const Vec2 DIRECTION_NW{-1, 1};
    const Vec2 DIRECTION_SE{1, -1};
    const Vec2 DIRECTION_SW{-1, -1};

    // Piece starting rows
    static constexpr int WHITE_BACK_ROW = 0;
    static constexpr int BLACK_BACK_ROW = 7;
    static constexpr int WHITE_PAWN_ROW = 1;
    static constexpr int BLACK_PAWN_ROW = 6;

    // Piece starting columns
    static constexpr int ROOK_COLUMNS[]   = {0, 7};
    static constexpr int KNIGHT_COLUMNS[] = {1, 6};
    static constexpr int BISHOP_COLUMNS[] = {2, 5};
    static constexpr int QUEEN_COLUMN     = 3;
    static constexpr int KING_COLUMN      = 4;

    // Cursor
    static constexpr float CURSOR_RADIUS = 0.1f;
}