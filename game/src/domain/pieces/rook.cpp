#include "domain/pieces/rook.hpp"
#include "domain/board.hpp"
#include <cstdlib>

namespace chess {

bool Rook::isValidMove(const Position& from, const Position& to, const Board& board) const {
    if (from.row != to.row && from.col != to.col) return false;

    int rowDir = (to.row - from.row) != 0 ? (to.row - from.row) / abs(to.row - from.row) : 0;
    int colDir = (to.col - from.col) != 0 ? (to.col - from.col) / abs(to.col - from.col) : 0;

    Position check = from;
    check.row += rowDir;
    check.col += colDir;

    while (check != to) {
        if (!board.isEmpty(check)) return false;
        check.row += rowDir;
        check.col += colDir;
    }

    auto target = board.getPieceAt(to);
    return !target || target->getColor() != color;
}

void Rook::markMoved() { 
    hasMoved = true;
}

bool Rook::hasMovedAlready() const {
    return hasMoved; 
}


}