#pragma once

#include "utils/utils.hpp"
#include "enums.hpp"
#include <memory>

namespace chess {

class Board;

class Piece {
protected:
    Color color;

public:
    explicit Piece(Color c) : color(c) {}
    virtual ~Piece() = default;

    Color getColor() const { return color; }
    virtual PieceType getType() const = 0;

    virtual bool isValidMove(const Position& from, const Position& to, const Board& board) const = 0;

    virtual std::shared_ptr<Piece> clone() const = 0;

    virtual char symbol() const = 0;
    virtual int getValue() const = 0;
};

}
