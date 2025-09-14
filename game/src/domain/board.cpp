#include "domain/board.hpp"
#include "domain/pieces/pawn.hpp"
#include "domain/pieces/rook.hpp"
#include "domain/pieces/knight.hpp"
#include "domain/pieces/bishop.hpp"
#include "domain/pieces/queen.hpp"
#include "domain/pieces/king.hpp"
#include <iostream>
#include <cstdlib>

namespace chess {

Board::Board(const Board& other) {
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            Position pos(r, c);
            const auto* piece = other.piece_at(pos);
            if (piece) {
                squares_[static_cast<size_t>(r)][static_cast<size_t>(c)] = piece->clone_unique();
            }
        }
    }
}

Board& Board::operator=(const Board& other) {
    if (this != &other) {
        for (int r = 0; r < 8; ++r) {
            for (int c = 0; c < 8; ++c) {
                Position pos(r, c);
                const auto* piece = other.piece_at(pos);
                if (piece) {
                    squares_[static_cast<size_t>(r)][static_cast<size_t>(c)] = piece->clone_unique();
                } else {
                    squares_[static_cast<size_t>(r)][static_cast<size_t>(c)].reset();
                }
            }
        }
    }
    return *this;
}

const Piece* Board::piece_at(Position pos) const noexcept {
    if (!pos.is_valid()) return nullptr;
    return squares_[static_cast<size_t>(pos.row)][static_cast<size_t>(pos.col)].get();
}

Piece* Board::piece_at(Position pos) noexcept {
    if (!pos.is_valid()) return nullptr;
    return squares_[static_cast<size_t>(pos.row)][static_cast<size_t>(pos.col)].get();
}

void Board::set_piece(Position pos, std::unique_ptr<Piece> piece) noexcept {
    if (pos.is_valid()) {
        squares_[static_cast<size_t>(pos.row)][static_cast<size_t>(pos.col)] = std::move(piece);
    }
}

void Board::move_piece(Position from, Position to) noexcept {
    if (!from.is_valid() || !to.is_valid()) return;

    auto piece = std::move(squares_[static_cast<size_t>(from.row)][static_cast<size_t>(from.col)]);
    if (!piece) return;

    // Handle en passant
    if (piece->getType() == PieceType::Pawn && is_empty(to) &&
        abs(to.col - from.col) == 1 && abs(to.row - from.row) == 1) {
        int capturedPawnRow = (piece->getColor() == Color::White) ? to.row - 1 : to.row + 1;
        Position capturedPos(capturedPawnRow, to.col);
        clear(capturedPos);
    }

    squares_[static_cast<size_t>(to.row)][static_cast<size_t>(to.col)] = std::move(piece);
    squares_[static_cast<size_t>(from.row)][static_cast<size_t>(from.col)].reset();
}

void Board::clear(Position pos) noexcept {
    if (pos.is_valid()) {
        squares_[static_cast<size_t>(pos.row)][static_cast<size_t>(pos.col)].reset();
    }
}

bool Board::is_empty(Position pos) const noexcept {
    return piece_at(pos) == nullptr;
}

bool Board::is_check(Color color) const noexcept {
    Position kingPos(-1, -1);
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            Position pos(r, c);
            const auto* piece = piece_at(pos);
            if (piece && piece->getType() == PieceType::King && piece->getColor() == color) {
                kingPos = pos;
                break;
            }
        }
        if (kingPos.row != -1) break;
    }

    if (kingPos.row == -1) return false; // King not found

    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            Position attackerPos(r, c);
            const auto* attacker = piece_at(attackerPos);
            if (attacker && attacker->getColor() != color) {
                if (attacker->isValidMove(attackerPos, kingPos, *this)) {
                    return true;
                }
            }
        }
    }

    return false;
}

void Board::setup_initial_position() {
    std::cout << "Setting up initial board position..." << std::endl;
    
    // Clear the board
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            squares_[static_cast<size_t>(r)][static_cast<size_t>(c)].reset();
        }
    }
    
    // Place pawns
    for (int col = 0; col < 8; ++col) {
        set_piece(Position(1, col), std::make_unique<Pawn>(Color::White));
        set_piece(Position(6, col), std::make_unique<Pawn>(Color::Black));
    }

    // Place back rank pieces
    std::array<std::unique_ptr<Piece>, 8> whiteBack = {
        std::make_unique<Rook>(Color::White),
        std::make_unique<Knight>(Color::White),
        std::make_unique<Bishop>(Color::White),
        std::make_unique<Queen>(Color::White),
        std::make_unique<King>(Color::White),
        std::make_unique<Bishop>(Color::White),
        std::make_unique<Knight>(Color::White),
        std::make_unique<Rook>(Color::White)
    };

    std::array<std::unique_ptr<Piece>, 8> blackBack = {
        std::make_unique<Rook>(Color::Black),
        std::make_unique<Knight>(Color::Black),
        std::make_unique<Bishop>(Color::Black),
        std::make_unique<Queen>(Color::Black),
        std::make_unique<King>(Color::Black),
        std::make_unique<Bishop>(Color::Black),
        std::make_unique<Knight>(Color::Black),
        std::make_unique<Rook>(Color::Black)
    };

    for (int col = 0; col < 8; ++col) {
        set_piece(Position(0, col), std::move(whiteBack[static_cast<size_t>(col)]));
        set_piece(Position(7, col), std::move(blackBack[static_cast<size_t>(col)]));
    }
    
    std::cout << "Board setup complete. Pieces placed:" << std::endl;
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            const auto* piece = piece_at({row, col});
            if (piece) {
                std::cout << "  (" << row << "," << col << "): " << piece->symbol() << std::endl;
            }
        }
    }
}

Board Board::clone() const {
    return Board(*this);
}

// Legacy interface implementations for compatibility
std::shared_ptr<Piece> Board::getPieceAt(const Position& pos) const {
    const auto* piece = piece_at(pos);
    return piece ? std::shared_ptr<Piece>(piece->clone()) : nullptr;
}

void Board::setPieceAt(const Position& pos, std::shared_ptr<Piece> piece) {
    set_piece(pos, piece ? piece->clone_unique() : nullptr);
}

void Board::movePiece(const Position& from, const Position& to) {
    move_piece(from, to);
}

void Board::setupInitialPosition() {
    setup_initial_position();
}

bool Board::isEmpty(const Position& pos) const {
    return is_empty(pos);
}


}
