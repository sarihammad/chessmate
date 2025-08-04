#pragma once
#include "../piece.hpp"

namespace chess {

class Knight : public Piece {
public:
    explicit Knight(Color c) : Piece(c) {}

    PieceType getType() const override { return PieceType::Knight; }

    bool isValidMove(const Position& from, const Position& to, const Board& board) const override;

    std::shared_ptr<Piece> clone() const override {
        return std::make_shared<Knight>(*this);
    }

    char symbol() const override {
        return color == Color::White ? 'N' : 'n';
    }
};

}