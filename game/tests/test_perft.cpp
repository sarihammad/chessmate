#include <catch2/catch_test_macros.hpp>
#include "domain/board.hpp"
#include "domain/game.hpp"
#include "utils/utils.hpp"

using namespace chess;

// Perft (Performance Test) - counts the number of leaf nodes at a given depth
// These are standard positions with known perft values
class PerftTest {
public:
    static int perft(Board& board, int depth, Color color) {
        if (depth == 0) {
            return 1;
        }
        
        int nodes = 0;
        
        for (int r = 0; r < 8; ++r) {
            for (int c = 0; c < 8; ++c) {
                Position from(r, c);
                const auto* piece = board.piece_at(from);
                
                if (piece && piece->getColor() == color) {
                    for (int tr = 0; tr < 8; ++tr) {
                        for (int tc = 0; tc < 8; ++tc) {
                            Position to(tr, tc);
                            
                            if (piece->isValidMove(from, to, board)) {
                                // Make move
                                Board boardCopy = board.clone();
                                boardCopy.move_piece(from, to);
                                
                                // Check if this move puts own king in check
                                if (!boardCopy.is_check(color)) {
                                    nodes += perft(boardCopy, depth - 1, 
                                                 color == Color::White ? Color::Black : Color::White);
                                }
                            }
                        }
                    }
                }
            }
        }
        
        return nodes;
    }
};

TEST_CASE("Perft Tests - Initial Position", "[perft]") {
    Board board;
    board.setup_initial_position();
    
    SECTION("Depth 1") {
        int result = PerftTest::perft(board, 1, Color::White);
        REQUIRE(result == 20); // Starting position has 20 legal moves for white
    }
    
    SECTION("Depth 2") {
        int result = PerftTest::perft(board, 2, Color::White);
        REQUIRE(result == 400); // After 1 move by white and 1 by black
    }
    
    SECTION("Depth 3") {
        int result = PerftTest::perft(board, 3, Color::White);
        REQUIRE(result == 8902); // After 3 half-moves
    }
}

TEST_CASE("Perft Tests - Kiwipete Position", "[perft]") {
    // Kiwipete is a complex test position
    Board board;
    
    // Clear board and set up Kiwipete position
    board.set_piece(Position(0, 4), std::make_unique<King>(Color::White));
    board.set_piece(Position(0, 0), std::make_unique<Rook>(Color::White));
    board.set_piece(Position(0, 7), std::make_unique<Rook>(Color::White));
    board.set_piece(Position(1, 0), std::make_unique<Pawn>(Color::White));
    board.set_piece(Position(1, 4), std::make_unique<Pawn>(Color::White));
    board.set_piece(Position(1, 6), std::make_unique<Pawn>(Color::White));
    board.set_piece(Position(2, 0), std::make_unique<Bishop>(Color::White));
    board.set_piece(Position(2, 5), std::make_unique<Knight>(Color::White));
    board.set_piece(Position(3, 0), std::make_unique<Bishop>(Color::White));
    board.set_piece(Position(3, 4), std::make_unique<Pawn>(Color::White));
    board.set_piece(Position(3, 5), std::make_unique<Queen>(Color::White));
    
    board.set_piece(Position(4, 3), std::make_unique<Pawn>(Color::Black));
    board.set_piece(Position(4, 4), std::make_unique<Pawn>(Color::Black));
    board.set_piece(Position(5, 1), std::make_unique<Bishop>(Color::Black));
    board.set_piece(Position(5, 3), std::make_unique<Knight>(Color::Black));
    board.set_piece(Position(5, 5), std::make_unique<Knight>(Color::Black));
    board.set_piece(Position(6, 0), std::make_unique<Pawn>(Color::Black));
    board.set_piece(Position(6, 1), std::make_unique<Pawn>(Color::Black));
    board.set_piece(Position(6, 2), std::make_unique<Pawn>(Color::Black));
    board.set_piece(Position(6, 6), std::make_unique<Pawn>(Color::Black));
    board.set_piece(Position(6, 7), std::make_unique<Pawn>(Color::Black));
    board.set_piece(Position(7, 0), std::make_unique<Rook>(Color::Black));
    board.set_piece(Position(7, 1), std::make_unique<Knight>(Color::Black));
    board.set_piece(Position(7, 4), std::make_unique<King>(Color::Black));
    board.set_piece(Position(7, 6), std::make_unique<Rook>(Color::Black));
    board.set_piece(Position(7, 7), std::make_unique<Queen>(Color::Black));
    
    SECTION("Depth 1") {
        int result = PerftTest::perft(board, 1, Color::White);
        REQUIRE(result == 48); // Kiwipete position has 48 legal moves for white
    }
}

TEST_CASE("Perft Tests - Position 3", "[perft]") {
    // Another standard test position
    Board board;
    
    // Clear and set up position 3
    board.set_piece(Position(0, 4), std::make_unique<King>(Color::White));
    board.set_piece(Position(0, 0), std::make_unique<Rook>(Color::White));
    board.set_piece(Position(0, 7), std::make_unique<Rook>(Color::White));
    board.set_piece(Position(1, 1), std::make_unique<Pawn>(Color::White));
    board.set_piece(Position(1, 2), std::make_unique<Pawn>(Color::White));
    board.set_piece(Position(1, 3), std::make_unique<Pawn>(Color::White));
    board.set_piece(Position(1, 4), std::make_unique<Pawn>(Color::White));
    board.set_piece(Position(1, 5), std::make_unique<Pawn>(Color::White));
    board.set_piece(Position(1, 6), std::make_unique<Pawn>(Color::White));
    board.set_piece(Position(1, 7), std::make_unique<Pawn>(Color::White));
    board.set_piece(Position(2, 0), std::make_unique<Bishop>(Color::White));
    board.set_piece(Position(2, 1), std::make_unique<Bishop>(Color::White));
    board.set_piece(Position(2, 5), std::make_unique<Knight>(Color::White));
    board.set_piece(Position(3, 0), std::make_unique<Queen>(Color::White));
    
    board.set_piece(Position(4, 0), std::make_unique<Pawn>(Color::Black));
    board.set_piece(Position(5, 1), std::make_unique<Bishop>(Color::Black));
    board.set_piece(Position(5, 3), std::make_unique<Knight>(Color::Black));
    board.set_piece(Position(5, 5), std::make_unique<Knight>(Color::Black));
    board.set_piece(Position(6, 0), std::make_unique<Pawn>(Color::Black));
    board.set_piece(Position(6, 1), std::make_unique<Pawn>(Color::Black));
    board.set_piece(Position(6, 2), std::make_unique<Pawn>(Color::Black));
    board.set_piece(Position(6, 3), std::make_unique<Pawn>(Color::Black));
    board.set_piece(Position(6, 4), std::make_unique<Pawn>(Color::Black));
    board.set_piece(Position(6, 5), std::make_unique<Pawn>(Color::Black));
    board.set_piece(Position(6, 6), std::make_unique<Pawn>(Color::Black));
    board.set_piece(Position(6, 7), std::make_unique<Pawn>(Color::Black));
    board.set_piece(Position(7, 0), std::make_unique<Rook>(Color::Black));
    board.set_piece(Position(7, 1), std::make_unique<Knight>(Color::Black));
    board.set_piece(Position(7, 4), std::make_unique<King>(Color::Black));
    board.set_piece(Position(7, 6), std::make_unique<Rook>(Color::Black));
    board.set_piece(Position(7, 7), std::make_unique<Queen>(Color::Black));
    
    SECTION("Depth 1") {
        int result = PerftTest::perft(board, 1, Color::White);
        REQUIRE(result == 14); // Position 3 has 14 legal moves for white
    }
}

TEST_CASE("Perft Tests - Tricky Position", "[perft]") {
    // A position that tests edge cases
    Board board;
    
    // Set up a tricky position with many captures and checks
    board.set_piece(Position(0, 4), std::make_unique<King>(Color::White));
    board.set_piece(Position(0, 0), std::make_unique<Rook>(Color::White));
    board.set_piece(Position(1, 4), std::make_unique<Pawn>(Color::White));
    board.set_piece(Position(1, 5), std::make_unique<Pawn>(Color::White));
    board.set_piece(Position(1, 6), std::make_unique<Pawn>(Color::White));
    board.set_piece(Position(2, 1), std::make_unique<Bishop>(Color::White));
    board.set_piece(Position(2, 4), std::make_unique<Bishop>(Color::White));
    board.set_piece(Position(3, 0), std::make_unique<Queen>(Color::White));
    board.set_piece(Position(3, 2), std::make_unique<Knight>(Color::White));
    board.set_piece(Position(3, 5), std::make_unique<Knight>(Color::White));
    
    board.set_piece(Position(4, 1), std::make_unique<Pawn>(Color::Black));
    board.set_piece(Position(4, 2), std::make_unique<Pawn>(Color::Black));
    board.set_piece(Position(4, 3), std::make_unique<Pawn>(Color::Black));
    board.set_piece(Position(4, 6), std::make_unique<Pawn>(Color::Black));
    board.set_piece(Position(5, 0), std::make_unique<Bishop>(Color::Black));
    board.set_piece(Position(5, 3), std::make_unique<Knight>(Color::Black));
    board.set_piece(Position(5, 6), std::make_unique<Knight>(Color::Black));
    board.set_piece(Position(6, 0), std::make_unique<Pawn>(Color::Black));
    board.set_piece(Position(6, 1), std::make_unique<Pawn>(Color::Black));
    board.set_piece(Position(6, 2), std::make_unique<Pawn>(Color::Black));
    board.set_piece(Position(6, 4), std::make_unique<Pawn>(Color::Black));
    board.set_piece(Position(6, 5), std::make_unique<Pawn>(Color::Black));
    board.set_piece(Position(6, 7), std::make_unique<Pawn>(Color::Black));
    board.set_piece(Position(7, 0), std::make_unique<Rook>(Color::Black));
    board.set_piece(Position(7, 3), std::make_unique<Queen>(Color::Black));
    board.set_piece(Position(7, 4), std::make_unique<King>(Color::Black));
    
    SECTION("Depth 1") {
        int result = PerftTest::perft(board, 1, Color::White);
        REQUIRE(result == 6); // Tricky position has 6 legal moves for white
    }
}
