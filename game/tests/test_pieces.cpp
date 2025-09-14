#include <catch2/catch_test_macros.hpp>
#include "domain/pieces/pawn.hpp"
#include "domain/pieces/rook.hpp"
#include "domain/pieces/knight.hpp"
#include "domain/pieces/bishop.hpp"
#include "domain/pieces/queen.hpp"
#include "domain/pieces/king.hpp"
#include "domain/board.hpp"
#include "utils/utils.hpp"

using namespace chess;

TEST_CASE("Pawn Movement", "[pieces][pawn]") {
    Board board;
    
    SECTION("White pawn initial move") {
        auto pawn = std::make_unique<Pawn>(Color::White);
        Position from(1, 4);
        Position to(3, 4);
        
        board.set_piece(from, std::move(pawn));
        
        REQUIRE(board.piece_at(from)->isValidMove(from, to, board));
    }
    
    SECTION("White pawn single move") {
        auto pawn = std::make_unique<Pawn>(Color::White);
        Position from(1, 4);
        Position to(2, 4);
        
        board.set_piece(from, std::move(pawn));
        
        REQUIRE(board.piece_at(from)->isValidMove(from, to, board));
    }
    
    SECTION("White pawn cannot move backward") {
        auto pawn = std::make_unique<Pawn>(Color::White);
        Position from(2, 4);
        Position to(1, 4);
        
        board.set_piece(from, std::move(pawn));
        
        REQUIRE_FALSE(board.piece_at(from)->isValidMove(from, to, board));
    }
    
    SECTION("White pawn diagonal capture") {
        auto pawn = std::make_unique<Pawn>(Color::White);
        auto enemyPawn = std::make_unique<Pawn>(Color::Black);
        
        Position from(1, 4);
        Position to(2, 5);
        
        board.set_piece(from, std::move(pawn));
        board.set_piece(to, std::move(enemyPawn));
        
        REQUIRE(board.piece_at(from)->isValidMove(from, to, board));
    }
    
    SECTION("White pawn cannot capture forward") {
        auto pawn = std::make_unique<Pawn>(Color::White);
        auto enemyPawn = std::make_unique<Pawn>(Color::Black);
        
        Position from(1, 4);
        Position to(2, 4);
        
        board.set_piece(from, std::move(pawn));
        board.set_piece(to, std::move(enemyPawn));
        
        REQUIRE_FALSE(board.piece_at(from)->isValidMove(from, to, board));
    }
    
    SECTION("Black pawn movement") {
        auto pawn = std::make_unique<Pawn>(Color::Black);
        Position from(6, 4);
        Position to(5, 4);
        
        board.set_piece(from, std::move(pawn));
        
        REQUIRE(board.piece_at(from)->isValidMove(from, to, board));
        
        // Black pawn initial double move
        Position toDouble(4, 4);
        REQUIRE(board.piece_at(from)->isValidMove(from, toDouble, board));
    }
}

TEST_CASE("Rook Movement", "[pieces][rook]") {
    Board board;
    
    SECTION("Rook horizontal movement") {
        auto rook = std::make_unique<Rook>(Color::White);
        Position from(4, 4);
        Position to(4, 0);
        
        board.set_piece(from, std::move(rook));
        
        REQUIRE(board.piece_at(from)->isValidMove(from, to, board));
    }
    
    SECTION("Rook vertical movement") {
        auto rook = std::make_unique<Rook>(Color::White);
        Position from(4, 4);
        Position to(0, 4);
        
        board.set_piece(from, std::move(rook));
        
        REQUIRE(board.piece_at(from)->isValidMove(from, to, board));
    }
    
    SECTION("Rook cannot move diagonally") {
        auto rook = std::make_unique<Rook>(Color::White);
        Position from(4, 4);
        Position to(2, 2);
        
        board.set_piece(from, std::move(rook));
        
        REQUIRE_FALSE(board.piece_at(from)->isValidMove(from, to, board));
    }
    
    SECTION("Rook blocked by piece") {
        auto rook = std::make_unique<Rook>(Color::White);
        auto pawn = std::make_unique<Pawn>(Color::White);
        
        Position from(4, 4);
        Position block(4, 2);
        Position to(4, 0);
        
        board.set_piece(from, std::move(rook));
        board.set_piece(block, std::move(pawn));
        
        REQUIRE_FALSE(board.piece_at(from)->isValidMove(from, to, board));
    }
    
    SECTION("Rook can capture enemy piece") {
        auto rook = std::make_unique<Rook>(Color::White);
        auto enemyPawn = std::make_unique<Pawn>(Color::Black);
        
        Position from(4, 4);
        Position to(4, 2);
        
        board.set_piece(from, std::move(rook));
        board.set_piece(to, std::move(enemyPawn));
        
        REQUIRE(board.piece_at(from)->isValidMove(from, to, board));
    }
}

TEST_CASE("Knight Movement", "[pieces][knight]") {
    Board board;
    
    SECTION("Knight L-shaped moves") {
        auto knight = std::make_unique<Knight>(Color::White);
        Position from(4, 4);
        
        board.set_piece(from, std::move(knight));
        
        // All 8 possible knight moves from center
        std::vector<Position> validMoves = {
            Position(2, 3), Position(2, 5),  // Up
            Position(6, 3), Position(6, 5),  // Down
            Position(3, 2), Position(5, 2),  // Left
            Position(3, 6), Position(5, 6)   // Right
        };
        
        for (const auto& to : validMoves) {
            REQUIRE(board.piece_at(from)->isValidMove(from, to, board));
        }
    }
    
    SECTION("Knight invalid moves") {
        auto knight = std::make_unique<Knight>(Color::White);
        Position from(4, 4);
        
        board.set_piece(from, std::move(knight));
        
        // Invalid moves (not L-shaped)
        std::vector<Position> invalidMoves = {
            Position(4, 2), Position(4, 6),  // Horizontal
            Position(2, 4), Position(6, 4),  // Vertical
            Position(2, 2), Position(6, 6)   // Diagonal
        };
        
        for (const auto& to : invalidMoves) {
            REQUIRE_FALSE(board.piece_at(from)->isValidMove(from, to, board));
        }
    }
    
    SECTION("Knight can jump over pieces") {
        auto knight = std::make_unique<Knight>(Color::White);
        auto pawn1 = std::make_unique<Pawn>(Color::White);
        auto pawn2 = std::make_unique<Pawn>(Color::Black);
        
        Position from(4, 4);
        Position block(4, 3);
        Position to(2, 3);
        
        board.set_piece(from, std::move(knight));
        board.set_piece(block, std::move(pawn1));
        
        REQUIRE(board.piece_at(from)->isValidMove(from, to, board));
        
        // Can capture enemy piece
        board.set_piece(to, std::move(pawn2));
        REQUIRE(board.piece_at(from)->isValidMove(from, to, board));
    }
}

TEST_CASE("Bishop Movement", "[pieces][bishop]") {
    Board board;
    
    SECTION("Bishop diagonal movement") {
        auto bishop = std::make_unique<Bishop>(Color::White);
        Position from(4, 4);
        
        board.set_piece(from, std::move(bishop));
        
        // All 4 diagonal directions
        std::vector<Position> validMoves = {
            Position(2, 2), Position(6, 6),  // Main diagonal
            Position(2, 6), Position(6, 2)   // Anti-diagonal
        };
        
        for (const auto& to : validMoves) {
            REQUIRE(board.piece_at(from)->isValidMove(from, to, board));
        }
    }
    
    SECTION("Bishop cannot move horizontally or vertically") {
        auto bishop = std::make_unique<Bishop>(Color::White);
        Position from(4, 4);
        
        board.set_piece(from, std::move(bishop));
        
        // Invalid moves
        std::vector<Position> invalidMoves = {
            Position(4, 0), Position(4, 7),  // Horizontal
            Position(0, 4), Position(7, 4)   // Vertical
        };
        
        for (const auto& to : invalidMoves) {
            REQUIRE_FALSE(board.piece_at(from)->isValidMove(from, to, board));
        }
    }
    
    SECTION("Bishop blocked by piece") {
        auto bishop = std::make_unique<Bishop>(Color::White);
        auto pawn = std::make_unique<Pawn>(Color::White);
        
        Position from(4, 4);
        Position block(2, 2);
        Position to(0, 0);
        
        board.set_piece(from, std::move(bishop));
        board.set_piece(block, std::move(pawn));
        
        REQUIRE_FALSE(board.piece_at(from)->isValidMove(from, to, board));
    }
}

TEST_CASE("Queen Movement", "[pieces][queen]") {
    Board board;
    
    SECTION("Queen combines rook and bishop moves") {
        auto queen = std::make_unique<Queen>(Color::White);
        Position from(4, 4);
        
        board.set_piece(from, std::move(queen));
        
        // Horizontal, vertical, and diagonal moves
        std::vector<Position> validMoves = {
            Position(4, 0), Position(4, 7),  // Horizontal
            Position(0, 4), Position(7, 4),  // Vertical
            Position(2, 2), Position(6, 6),  // Diagonal
            Position(2, 6), Position(6, 2)   // Anti-diagonal
        };
        
        for (const auto& to : validMoves) {
            REQUIRE(board.piece_at(from)->isValidMove(from, to, board));
        }
    }
    
    SECTION("Queen blocked by pieces") {
        auto queen = std::make_unique<Queen>(Color::White);
        auto pawn = std::make_unique<Pawn>(Color::White);
        
        Position from(4, 4);
        Position block(4, 2);
        Position to(4, 0);
        
        board.set_piece(from, std::move(queen));
        board.set_piece(block, std::move(pawn));
        
        REQUIRE_FALSE(board.piece_at(from)->isValidMove(from, to, board));
    }
}

TEST_CASE("King Movement", "[pieces][king]") {
    Board board;
    
    SECTION("King moves one square in any direction") {
        auto king = std::make_unique<King>(Color::White);
        Position from(4, 4);
        
        board.set_piece(from, std::move(king));
        
        // All 8 surrounding squares
        std::vector<Position> validMoves = {
            Position(3, 3), Position(3, 4), Position(3, 5),  // Up
            Position(4, 3),                 Position(4, 5),  // Middle
            Position(5, 3), Position(5, 4), Position(5, 5)   // Down
        };
        
        for (const auto& to : validMoves) {
            REQUIRE(board.piece_at(from)->isValidMove(from, to, board));
        }
    }
    
    SECTION("King cannot move more than one square") {
        auto king = std::make_unique<King>(Color::White);
        Position from(4, 4);
        
        board.set_piece(from, std::move(king));
        
        // Invalid moves (more than one square)
        std::vector<Position> invalidMoves = {
            Position(2, 4), Position(6, 4),  // Vertical
            Position(4, 2), Position(4, 6),  // Horizontal
            Position(2, 2), Position(6, 6)   // Diagonal
        };
        
        for (const auto& to : invalidMoves) {
            REQUIRE_FALSE(board.piece_at(from)->isValidMove(from, to, board));
        }
    }
    
    SECTION("King can capture enemy pieces") {
        auto king = std::make_unique<King>(Color::White);
        auto enemyPawn = std::make_unique<Pawn>(Color::Black);
        
        Position from(4, 4);
        Position to(3, 4);
        
        board.set_piece(from, std::move(king));
        board.set_piece(to, std::move(enemyPawn));
        
        REQUIRE(board.piece_at(from)->isValidMove(from, to, board));
    }
}
