#include "domain/pieces/pawn.hpp"
#include "domain/board.hpp"

namespace chess {

bool Pawn::isValidMove(const Position& from, const Position& to, const Board& board) const {
    int direction = (color == Color::White) ? 1 : -1;
    int startRow = (color == Color::White) ? 1 : 6;

    int rowDiff = to.row - from.row;
    int colDiff = std::abs(to.col - from.col);

    if (colDiff == 0) {
        if (rowDiff == direction && board.isEmpty(to)) return true;
        if (from.row == startRow && rowDiff == 2 * direction) {
            Position intermediate(from.row + direction, from.col);
            return board.isEmpty(intermediate) && board.isEmpty(to);
        }
    }

    if (colDiff == 1 && rowDiff == direction) {
        auto target = board.getPieceAt(to);
        if (target) {
            return target->getColor() != color;
        }

        // en passant
        Position adjacent(from.row, to.col);
        auto adjacentPawn = board.getPieceAt(adjacent);
        if (adjacentPawn &&
            adjacentPawn->getType() == PieceType::Pawn &&
            adjacentPawn->getColor() != color) {
            auto pawnPtr = std::dynamic_pointer_cast<Pawn>(adjacentPawn);
            if (pawnPtr && pawnPtr->isEnPassantCapturable() && board.isEmpty(to)) {
                return true;
            }
        }

    }

    return false;
}

}
