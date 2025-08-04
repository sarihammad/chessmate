#include "infrastructure/players/human_player.hpp"

namespace chess {

HumanPlayer::HumanPlayer(Color c) : Player(c) {}

bool HumanPlayer::makeMove(Board& board, Position& from, Position& to) {
    if (!from.isValid() || !to.isValid()) return false;

    auto piece = board.getPieceAt(from);
    if (!piece || piece->getColor() != color) return false;
    if (!piece->isValidMove(from, to, board)) return false;

    board.movePiece(from, to);
    return true;
}

}
