#include "domain/pieces/king.hpp"
#include "domain/board.hpp"
#include <cstdlib>

namespace chess {

bool King::isValidMove(const Position& from, const Position& to, const Board& board) const {
    int rowDiff = abs(to.row - from.row);
    int colDiff = abs(to.col - from.col);

    if (rowDiff <= 1 && colDiff <= 1) {
        auto target = board.getPieceAt(to);
        return !target || target->getColor() != color;
    }

    if (!hasMovedAlready() && from.row == to.row && colDiff == 2) {
        int direction = (to.col > from.col) ? 1 : -1;
        int rookCol = (direction == 1) ? 7 : 0;
        Position rookPos(from.row, rookCol);
        auto rook = board.getPieceAt(rookPos);

        if (rook && rook->getType() == PieceType::Rook && rook->getColor() == color) {
            auto rookMovable = std::dynamic_pointer_cast<MovablePiece>(rook);
            if (!rookMovable || rookMovable->hasMovedAlready()) return false;

            // check all squares between king and rook are empty
            for (int col = from.col + direction; col != rookCol; col += direction) {
                if (!board.isEmpty(Position(from.row, col))) return false;
            }

            // ensure path is not under check
            for (int step = 0; step <= 2; ++step) {
                Position intermediate(from.row, from.col + step * direction);
                Board tempBoard = board.clone();
                tempBoard.movePiece(from, intermediate);

                // check if king would still be in check after this move
                if (tempBoard.getPieceAt(intermediate) &&
                    tempBoard.getPieceAt(intermediate)->getType() == PieceType::King &&
                    tempBoard.is_check(color)) {
                    return false;
                }
            }

            return true;
        }
    }

    return false;
}

void King::markMoved() { 
    hasMoved = true;
}

bool King::hasMovedAlready() const {
    return hasMoved; 
}

}