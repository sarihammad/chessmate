#pragma once
#include "../piece.hpp"
#include "movable_piece.hpp"

namespace chess {

class King : public Piece, public MovablePiece {
private:
    bool hasMoved = false;
    
public:
    explicit King(Color c) : Piece(c) {}

    PieceType getType() const override { return PieceType::King; }

    bool isValidMove(const Position& from, const Position& to, const Board& board) const override;

    std::shared_ptr<Piece> clone() const override {
        return std::make_shared<King>(*this);
    }
    
    std::unique_ptr<Piece> clone_unique() const override {
        return std::make_unique<King>(*this);
    }

    char symbol() const override {
        return color == Color::White ? 'K' : 'k';
    }

    int getValue() const override { return 900; }

    void markMoved() override;

    bool hasMovedAlready() const override;
};

}