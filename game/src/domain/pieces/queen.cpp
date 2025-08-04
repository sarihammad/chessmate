#include "domain/pieces/queen.hpp"
#include "domain/board.hpp"
#include "domain/pieces/rook.hpp"
#include "domain/pieces/bishop.hpp"

namespace chess {

bool Queen::isValidMove(const Position& from, const Position& to, const Board& board) const {
    int rowDiff = std::abs(to.row - from.row);
    int colDiff = std::abs(to.col - from.col);

    int rowDir = (to.row - from.row) == 0 ? 0 : ((to.row - from.row) > 0 ? 1 : -1);
    int colDir = (to.col - from.col) == 0 ? 0 : ((to.col - from.col) > 0 ? 1 : -1);

    // Not a straight or diagonal line
    if (from.row != to.row && from.col != to.col && rowDiff != colDiff)
        return false;

    int r = from.row + rowDir;
    int c = from.col + colDir;

    while (r != to.row || c != to.col) {
        if (!board.isEmpty(Position(r, c)))
            return false;
        r += rowDir;
        c += colDir;
    }

    auto target = board.getPieceAt(to);
    return !target || target->getColor() != color;
}

}