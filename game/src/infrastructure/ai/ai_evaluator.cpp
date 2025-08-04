#include "infrastructure/ai/ai_evaluator.hpp"
#include <memory>

namespace chess {

const double pawnTable[8][8] = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {5, 5, 5,-5, -5, 5, 5, 5},
    {1, 1, 2, 3, 3, 2, 1, 1},
    {0, 0, 0, 2, 2, 0, 0, 0},
    {1, 1, 1, -2, -2, 1, 1, 1},
    {1, 2, 2, -2, -2, 2, 2, 1},
    {5, 5, 5, -5, -5, 5, 5, 5},
    {0, 0, 0, 0, 0, 0, 0, 0}
};

const double knightTable[8][8] = {
    {-5,-4,-3,-3,-3,-3,-4,-5},
    {-4,-2, 0, 0, 0, 0,-2,-4},
    {-3, 0, 1, 1.5, 1.5, 1, 0,-3},
    {-3, 0.5, 1.5, 2, 2, 1.5, 0.5,-3},
    {-3, 0, 1.5, 2, 2, 1.5, 0,-3},
    {-3, 0.5, 1, 1.5, 1.5, 1, 0.5,-3},
    {-4,-2, 0, 0.5, 0.5, 0, -2,-4},
    {-5,-4,-3,-3,-3,-3,-4,-5}
};

const double bishopTable[8][8] = {
    {-2,-1,-1,-1,-1,-1,-1,-2},
    {-1, 0, 0, 0, 0, 0, 0,-1},
    {-1, 0, 0.5, 1, 1, 0.5, 0,-1},
    {-1, 0.5, 0.5, 1, 1, 0.5, 0.5,-1},
    {-1, 0, 1, 1, 1, 1, 0,-1},
    {-1, 1, 1, 1, 1, 1, 1,-1},
    {-1, 0.5, 0, 0, 0, 0, 0.5,-1},
    {-2,-1,-1,-1,-1,-1,-1,-2}
};

const double rookTable[8][8] = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0.5, 1, 1, 1, 1, 1, 1, 0.5},
    {-0.5, 0, 0, 0, 0, 0, 0, -0.5},
    {-0.5, 0, 0, 0, 0, 0, 0, -0.5},
    {-0.5, 0, 0, 0, 0, 0, 0, -0.5},
    {-0.5, 0, 0, 0, 0, 0, 0, -0.5},
    {-0.5, 0, 0, 0, 0, 0, 0, -0.5},
    {0, 0, 0, 0.5, 0.5, 0, 0, 0}
};

const double queenTable[8][8] = {
    {-2,-1,-1, -0.5, -0.5,-1,-1,-2},
    {-1, 0, 0, 0, 0, 0, 0,-1},
    {-1, 0, 0.5, 0.5, 0.5, 0.5, 0,-1},
    {-0.5, 0, 0.5, 0.5, 0.5, 0.5, 0,-0.5},
    {0, 0, 0.5, 0.5, 0.5, 0.5, 0, -0.5},
    {-1, 0.5, 0.5, 0.5, 0.5, 0.5, 0,-1},
    {-1, 0, 0.5, 0, 0, 0, 0,-1},
    {-2,-1,-1, -0.5, -0.5,-1,-1,-2}
};

const double kingTable[8][8] = {
    {-3,-4,-4,-5,-5,-4,-4,-3},
    {-3,-4,-4,-5,-5,-4,-4,-3},
    {-3,-4,-4,-5,-5,-4,-4,-3},
    {-3,-4,-4,-5,-5,-4,-4,-3},
    {-2,-3,-3,-4,-4,-3,-3,-2},
    {-1,-2,-2,-2,-2,-2,-2,-1},
    {2, 2, 0, 0, 0, 0, 2, 2},
    {2, 3, 1, 0, 0, 1, 3, 2}
};

int pieceValue(PieceType type) {
    switch (type) {
        case PieceType::Pawn: return 10;
        case PieceType::Knight: return 30;
        case PieceType::Bishop: return 30;
        case PieceType::Rook: return 50;
        case PieceType::Queen: return 90;
        case PieceType::King: return 900;
        default: return 0;
    }
}

int evaluateBoard(const Board& board, Color aiColor) {
    int score = 0;

    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            Position pos(r, c);
            auto piece = board.getPieceAt(pos);
            if (!piece) continue;

            int base = pieceValue(piece->getType());
            int modifier = (piece->getColor() == aiColor) ? 1 : -1;

            // Mirror row index if black so table aligns
            int row = (piece->getColor() == Color::White) ? r : 7 - r;
            int col = c;

            score += base * modifier;

            switch (piece->getType()) {
                case PieceType::Pawn:
                    score += modifier * pawnTable[row][col];
                    break;
                case PieceType::Knight:
                    score += modifier * knightTable[row][col];
                    break;
                case PieceType::Bishop:
                    score += modifier * bishopTable[row][col];
                    break;
                case PieceType::Rook:
                    score += modifier * rookTable[row][col];
                    break;
                case PieceType::Queen:
                    score += modifier * queenTable[row][col];
                    break;
                case PieceType::King: {
                    score += modifier * kingTable[row][col];

                    // simple king safety
                    int defenders = 0;
                    for (int dr = -1; dr <= 1; ++dr) {
                        for (int dc = -1; dc <= 1; ++dc) {
                            int nr = r + dr, nc = c + dc;
                            if (nr >= 0 && nr < 8 && nc >= 0 && nc < 8) {
                                auto neighbor = board.getPieceAt(Position(nr, nc));
                                if (neighbor && neighbor->getColor() == piece->getColor())
                                    defenders++;
                            }
                        }
                    }
                    score += modifier * defenders * 2;
                    break;
                }
                default:
                    break;
            }
        }
    }

    return score;
}


}