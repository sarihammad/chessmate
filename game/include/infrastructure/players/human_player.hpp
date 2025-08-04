#pragma once

#include "domain/player.hpp"

namespace chess {

class HumanPlayer : public Player {
public:
    explicit HumanPlayer(Color c);
    bool makeMove(Board& board, Position& from, Position& to) override;
};

}