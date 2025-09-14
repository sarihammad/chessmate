#include "domain/pieces/bishop.hpp"
#include "domain/board.hpp"
#include <cstdlib>

namespace chess {

bool Bishop::isValidMove(const Position& from, const Position& to, const Board& board) const {
    int rowDiff = abs(to.row - from.row);
    int colDiff = abs(to.col - from.col);
    if (rowDiff != colDiff) return false;

    int rowDir = (to.row - from.row) > 0 ? 1 : -1;
    int colDir = (to.col - from.col) > 0 ? 1 : -1;

    for (int i = 1; i < rowDiff; ++i) {
        int r = from.row + i * rowDir;
        int c = from.col + i * colDir;
        if (!board.isEmpty(Position(r, c))) return false;
    }

    auto target = board.getPieceAt(to);
    return !target || target->getColor() != color;
}

}