#pragma once

#include "domain/enums.hpp"
#include "domain/board.hpp"
#include "utils/utils.hpp"
// #include "move.hpp"
#include "gamestate.hpp"
#include <memory>

namespace chess {

class Player {
protected:
    Color color;

public:
    explicit Player(Color c) : color(c) {}
    virtual ~Player() = default;

    Color getColor() const { return color; }

    virtual bool makeMove(Board& board, Position& from, Position& to) = 0;
};

}
