#pragma once

#include "utils/utils.hpp"
#include "piece.hpp"
#include <vector>
#include <memory>
#include <optional>

namespace chess {

class Board {
private:
    std::vector<std::vector<std::shared_ptr<Piece>>> squares;

public:
    Board();
    std::shared_ptr<Piece> getPieceAt(const Position& pos) const;
    void setPieceAt(const Position& pos, std::shared_ptr<Piece> piece);
    void movePiece(const Position& from, const Position& to);
    void setupInitialPosition();
    Board clone() const;

    bool isEmpty(const Position& pos) const;

    bool isCheck(Color color) const;
};

}
