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
    
    std::unique_ptr<Piece> clone_unique() const override {
        return std::make_unique<Knight>(*this);
    }

    char symbol() const override {
        return color == Color::White ? 'N' : 'n';
    }

    int getValue() const override { return 30; }
};

}