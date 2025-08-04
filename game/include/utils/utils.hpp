#pragma once
#include "domain/enums.hpp"

namespace chess {

struct Position {
    int row = 0;
    int col = 0;

    Position() = default;
    Position(int r, int c) : row(r), col(c) {}

    bool isValid() const;

    bool operator==(const Position& other) const;

    bool operator!=(const Position& other) const;

    bool operator<(const Position& other) const;
};

Color getOtherColor(Color c);

struct Move {
    Position from;
    Position to;

    Move(Position from, Position to) : from(from), to(to) {}
    Move() = default;
};

}

