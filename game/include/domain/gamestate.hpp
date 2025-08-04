#pragma once

#include "board.hpp"
#include "enums.hpp"

namespace chess {

class GameState {
public:
    Board board;
    Color currentTurn;

    GameState(const Board& b, Color turn)
        : board(b), currentTurn(turn) {}

    const Board& getBoard() const { return board; }
    Color getTurn() const { return currentTurn; }
};

}