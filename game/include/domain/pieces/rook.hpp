#pragma once

#include "../piece.hpp"
#include "movable_piece.hpp"

namespace chess {

class Rook : public Piece, public MovablePiece {
private:
    bool hasMoved = false;

public:
    explicit Rook(Color c) : Piece(c) {}

    PieceType getType() const override { return PieceType::Rook; }

    bool isValidMove(const Position& from, const Position& to, const Board& board) const override;

    std::shared_ptr<Piece> clone() const override {
        return std::make_shared<Rook>(*this);
    }
    
    std::unique_ptr<Piece> clone_unique() const override {
        return std::make_unique<Rook>(*this);
    }

    char symbol() const override {
        return color == Color::White ? 'R' : 'r';
    }

    int getValue() const override { return 50; }

    void markMoved() override;
    bool hasMovedAlready() const override;
};

}
