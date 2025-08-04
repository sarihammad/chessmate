#pragma once
#include "domain/board.hpp"
#include "domain/enums.hpp"

namespace chess {

extern const double pawnTable[8][8];
extern const double knightTable[8][8];
extern const double bishopTable[8][8];
extern const double rookTable[8][8];
extern const double queenTable[8][8];
extern const double kingTable[8][8];

int evaluateBoard(const Board& board, Color aiColor);

}
