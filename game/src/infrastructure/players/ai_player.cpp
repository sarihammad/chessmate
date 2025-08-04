#include "infrastructure/players/ai_player.hpp"
#include "domain/pieces/queen.hpp"
#include <limits>
#include <vector>
#include <iostream> // Added for debug output

namespace chess {

bool AIPlayer::makeMove(Board& board, Position& from, Position& to) {
    std::cout << "DEBUG: AI makeMove called for color " << (color == Color::White ? "White" : "Black") << std::endl;
    Position bestFrom(-1, -1), bestTo(-1, -1);
    minimax(board, 3, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), true, bestFrom, bestTo);

    std::cout << "DEBUG: AI minimax returned bestFrom: (" << bestFrom.row << ", " << bestFrom.col 
              << ") bestTo: (" << bestTo.row << ", " << bestTo.col << ")" << std::endl;

    if (bestFrom.isValid() && bestTo.isValid()) {
        from = bestFrom;
        to = bestTo;
        std::cout << "DEBUG: AI returning valid move" << std::endl;
        return true;
    }

    std::cout << "DEBUG: AI failed to find valid move" << std::endl;
    return false;
}

int AIPlayer::minimax(Board& board, int depth, int alpha, int beta, bool maximizing, Position& bestFrom, Position& bestTo) {
    if (depth == 0) {
        return evaluateBoard(board, color);
    }

    int bestScore = maximizing ? std::numeric_limits<int>::min() : std::numeric_limits<int>::max();
    bool foundAnyMove = false;

    for (int r1 = 0; r1 < 8; ++r1) {
        for (int c1 = 0; c1 < 8; ++c1) {
            Position from(r1, c1);
            auto piece = board.getPieceAt(from);
            if (!piece || piece->getColor() != (maximizing ? color : getOtherColor(color))) continue;

            for (int r2 = 0; r2 < 8; ++r2) {
                for (int c2 = 0; c2 < 8; ++c2) {
                    Position to(r2, c2);
                    if (!piece->isValidMove(from, to, board)) continue;

                    foundAnyMove = true;
                    std::cout << "DEBUG: AI found valid move from (" << from.row << ", " << from.col 
                              << ") to (" << to.row << ", " << to.col << ") for " 
                              << (maximizing ? "maximizing" : "minimizing") << " player" << std::endl;

                    // Use a board copy for simulation to avoid modifying the original
                    Board boardCopy = board.clone();
                    auto captured = boardCopy.getPieceAt(to);
                    boardCopy.movePiece(from, to);

                    int eval = minimax(boardCopy, depth - 1, alpha, beta, !maximizing, bestFrom, bestTo);

                    if (maximizing) {
                        if (eval > bestScore) {
                            bestScore = eval;
                            if (depth == 3) {
                                bestFrom = from;
                                bestTo = to;
                                std::cout << "DEBUG: AI found better maximizing move: (" << from.row << ", " << from.col 
                                          << ") to (" << to.row << ", " << to.col << ") with score " << eval << std::endl;
                            }
                        }
                        alpha = std::max(alpha, eval);
                        if (beta <= alpha) break;
                    } else {
                        bestScore = std::min(bestScore, eval);
                        beta = std::min(beta, eval);
                        if (beta <= alpha) break;
                    }
                }
            }
        }
    }

    if (!foundAnyMove) {
        std::cout << "DEBUG: AI found no valid moves for " << (maximizing ? "maximizing" : "minimizing") << " player" << std::endl;
    }

    return bestScore;
}

}
