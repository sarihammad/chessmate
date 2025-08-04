#pragma once

#include "domain/player.hpp"
#include "../ai/ai_evaluator.hpp"

namespace chess {

class AIPlayer : public Player {
public:
    explicit AIPlayer(Color c) : Player(c) {}

    bool makeMove(Board& board, Position& from, Position& to) override;

private:
    int minimax(Board& board, int depth, int alpha, int beta, bool maximizing, Position& bestFrom, Position& bestTo);
};

}
