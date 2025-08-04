#include <cassert>
#include <iostream>
#include <domain/board.hpp>
#include <domain/piece.hpp>
#include <domain/game.hpp>
#include <utils/utils.hpp>

using namespace chess;

void test_board_piece_placement() {
    Board board;
    auto pawn = std::make_shared<Piece>(PieceType::Pawn, Color::White);
    Position pos(1, 1);
    board.setPieceAt(pos, pawn);
    assert(board.getPieceAt(pos) == pawn);
}

void test_board_move_piece() {
    Board board;
    auto rook = std::make_shared<Piece>(PieceType::Rook, Color::White);
    Position from(0, 0), to(0, 7);
    board.setPieceAt(from, rook);
    board.movePiece(from, to);
    assert(board.getPieceAt(to) == rook);
    assert(board.getPieceAt(from) == nullptr);
}

void test_game_over_detection() {
    Game game;
    // Simulate a checkmate or stalemate position if possible
    // For now, just check that a new game is not over
    assert(!game.isGameOver());
}

int main() {
    test_board_piece_placement();
    test_board_move_piece();
    test_game_over_detection();
    std::cout << "All domain tests passed!\n";
    return 0;
} 