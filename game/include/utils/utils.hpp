#pragma once
#include "domain/enums.hpp"
#include <compare>

namespace chess {

struct Position {
    int row = 0;
    int col = 0;

    constexpr Position() = default;
    constexpr Position(int r, int c) noexcept : row(r), col(c) {}

    [[nodiscard]] constexpr bool is_valid() const noexcept {
        return row >= 0 && row < 8 && col >= 0 && col < 8;
    }
    
    [[nodiscard]] constexpr bool on_board() const noexcept {
        return is_valid();
    }

    // Legacy interface for compatibility
    [[nodiscard]] bool isValid() const { return is_valid(); }

    [[nodiscard]] constexpr bool operator==(const Position& other) const noexcept = default;
    [[nodiscard]] constexpr bool operator!=(const Position& other) const noexcept = default;
    [[nodiscard]] constexpr bool operator<(const Position& other) const noexcept {
        return row < other.row || (row == other.row && col < other.col);
    }
    
    [[nodiscard]] constexpr std::strong_ordering operator<=>(const Position& other) const noexcept {
        if (auto cmp = row <=> other.row; cmp != 0) return cmp;
        return col <=> other.col;
    }
};

Color getOtherColor(Color c);

struct Move {
    Position from;
    Position to;

    Move(Position from, Position to) : from(from), to(to) {}
    Move() = default;
};

}

