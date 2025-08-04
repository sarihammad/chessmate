#pragma once
#include "../piece.hpp"

namespace chess {

class Queen : public Piece {
public:
    explicit Queen(Color c) : Piece(c) {}

    PieceType getType() const override { return PieceType::Queen; }

    bool isValidMove(const Position& from, const Position& to, const Board& board) const override;

    std::shared_ptr<Piece> clone() const override {
        return std::make_shared<Queen>(*this);
    }

    char symbol() const override {
        return color == Color::White ? 'Q' : 'q';
    }
};

}