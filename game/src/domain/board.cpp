#include "domain/board.hpp"
#include "domain/pieces/pawn.hpp"
#include "domain/pieces/rook.hpp"
#include "domain/pieces/knight.hpp"
#include "domain/pieces/bishop.hpp"
#include "domain/pieces/queen.hpp"
#include "domain/pieces/king.hpp"
#include <iostream>

namespace chess {

Board::Board() : squares(8, std::vector<std::shared_ptr<Piece>>(8, nullptr)) {}

std::shared_ptr<Piece> Board::getPieceAt(const Position& pos) const {
    if (!pos.isValid()) return nullptr;
    return squares[pos.row][pos.col];
}

Board Board::clone() const {
    Board copy;
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            Position pos(r, c);
            auto piece = getPieceAt(pos);
            if (piece) {
                copy.setPieceAt(pos, piece->clone());
            }
        }
    }
    return copy;
}

void Board::setPieceAt(const Position& pos, std::shared_ptr<Piece> piece) {
    if (pos.isValid())
        squares[pos.row][pos.col] = piece;
}

void Board::movePiece(const Position& from, const Position& to) {
    if (!from.isValid() || !to.isValid()) return;

    auto piece = getPieceAt(from);

    // en passant
    if (piece && piece->getType() == PieceType::Pawn && isEmpty(to) &&
        std::abs(to.col - from.col) == 1 && std::abs(to.row - from.row) == 1) {
        // diagonal move to empty square -> en passant
        int capturedPawnRow = (piece->getColor() == Color::White) ? to.row - 1 : to.row + 1;
        Position capturedPos(capturedPawnRow, to.col);
        setPieceAt(capturedPos, nullptr);
    }

    setPieceAt(to, piece);
    setPieceAt(from, nullptr);
}

bool Board::isEmpty(const Position& pos) const {
    return getPieceAt(pos) == nullptr;
}

void Board::setupInitialPosition() {
    std::cout << "Setting up initial board position..." << std::endl;
    
    for (int col = 0; col < 8; ++col) {
        setPieceAt(Position(1, col), std::make_shared<Pawn>(Color::White));
        setPieceAt(Position(6, col), std::make_shared<Pawn>(Color::Black));
    }

    std::vector<std::shared_ptr<Piece>> whiteBack = {
        std::make_shared<Rook>(Color::White),
        std::make_shared<Knight>(Color::White),
        std::make_shared<Bishop>(Color::White),
        std::make_shared<Queen>(Color::White),
        std::make_shared<King>(Color::White),
        std::make_shared<Bishop>(Color::White),
        std::make_shared<Knight>(Color::White),
        std::make_shared<Rook>(Color::White)
    };

    std::vector<std::shared_ptr<Piece>> blackBack = {
        std::make_shared<Rook>(Color::Black),
        std::make_shared<Knight>(Color::Black),
        std::make_shared<Bishop>(Color::Black),
        std::make_shared<Queen>(Color::Black),
        std::make_shared<King>(Color::Black),
        std::make_shared<Bishop>(Color::Black),
        std::make_shared<Knight>(Color::Black),
        std::make_shared<Rook>(Color::Black)
    };

    for (int col = 0; col < 8; ++col) {
        setPieceAt(Position(0, col), whiteBack[col]);
        setPieceAt(Position(7, col), blackBack[col]);
    }
    
    std::cout << "Board setup complete. Pieces placed:" << std::endl;
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            auto piece = getPieceAt({row, col});
            if (piece) {
                std::cout << "  (" << row << "," << col << "): " << piece->symbol() << std::endl;
            }
        }
    }
}

bool Board::isCheck(Color color) const {
    Position kingPos(-1, -1);
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            Position pos(r, c);
            auto piece = getPieceAt(pos);
            if (piece && piece->getType() == PieceType::King && piece->getColor() == color)
                kingPos = pos;
        }
    }

    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            Position attackerPos(r, c);
            auto attacker = getPieceAt(attackerPos);
            if (attacker && attacker->getColor() != color) {
                if (attacker->isValidMove(attackerPos, kingPos, *this))
                    return true;
            }
        }
    }

    return false;
}

}
