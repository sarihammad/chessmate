#pragma once
#include "../piece.hpp"

namespace chess {

class Bishop : public Piece {
public:
    explicit Bishop(Color c) : Piece(c) {}

    PieceType getType() const override { return PieceType::Bishop; }

    bool isValidMove(const Position& from, const Position& to, const Board& board) const override;

    std::shared_ptr<Piece> clone() const override {
        return std::make_shared<Bishop>(*this);
    }

    char symbol() const override {
        return color == Color::White ? 'B' : 'b';
    }
};

}