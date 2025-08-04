#pragma once

#include "../piece.hpp"

namespace chess {

class Pawn : public Piece {
public:
    explicit Pawn(Color c) : Piece(c) {}
    PieceType getType() const override { return PieceType::Pawn; }

    bool isValidMove(const Position& from, const Position& to, const Board& board) const override;

    std::shared_ptr<Piece> clone() const override {
        return std::make_shared<Pawn>(*this);
    }

    char symbol() const override {
        return color == Color::White ? 'P' : 'p';
    }

    void setEnPassantCapturable(bool value) { 
        enPassantCapturable = value; 
    }
    
    bool isEnPassantCapturable() const { 
        return enPassantCapturable; 
    }

private:
    bool enPassantCapturable = false;
};

}