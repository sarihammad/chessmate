#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include "domain/board.hpp"
#include "domain/pieces/pawn.hpp"
#include "domain/pieces/rook.hpp"
#include "domain/pieces/knight.hpp"
#include "domain/pieces/bishop.hpp"
#include "domain/pieces/queen.hpp"
#include "domain/pieces/king.hpp"
#include "utils/utils.hpp"

using namespace chess;

TEST_CASE("Board Construction and Basic Operations", "[board]") {
    Board board;
    
    SECTION("Board starts empty") {
        for (int r = 0; r < 8; ++r) {
            for (int c = 0; c < 8; ++c) {
                Position pos(r, c);
                REQUIRE(board.is_empty(pos));
                REQUIRE(board.piece_at(pos) == nullptr);
            }
        }
    }
    
    SECTION("Setting and getting pieces") {
        Position pos(3, 4);
        auto pawn = std::make_unique<Pawn>(Color::White);
        Piece* pawn_ptr = pawn.get();
        
        board.set_piece(pos, std::move(pawn));
        
        REQUIRE_FALSE(board.is_empty(pos));
        REQUIRE(board.piece_at(pos) == pawn_ptr);
        REQUIRE(board.piece_at(pos)->getType() == PieceType::Pawn);
        REQUIRE(board.piece_at(pos)->getColor() == Color::White);
    }
    
    SECTION("Moving pieces") {
        Position from(1, 0);
        Position to(3, 0);
        
        auto pawn = std::make_unique<Pawn>(Color::White);
        board.set_piece(from, std::move(pawn));
        
        board.move_piece(from, to);
        
        REQUIRE(board.is_empty(from));
        REQUIRE_FALSE(board.is_empty(to));
        REQUIRE(board.piece_at(to)->getType() == PieceType::Pawn);
    }
    
    SECTION("Invalid positions") {
        Position invalid(-1, 0);
        Position valid(0, 0);
        
        REQUIRE_FALSE(invalid.is_valid());
        REQUIRE(valid.is_valid());
        
        auto pawn = std::make_unique<Pawn>(Color::White);
        board.set_piece(invalid, std::move(pawn));
        
        REQUIRE(board.is_empty(invalid));
        REQUIRE(board.piece_at(invalid) == nullptr);
    }
}

TEST_CASE("Board Copy Operations", "[board]") {
    Board original;
    original.setup_initial_position();
    
    SECTION("Copy constructor") {
        Board copy(original);
        
        for (int r = 0; r < 8; ++r) {
            for (int c = 0; c < 8; ++c) {
                Position pos(r, c);
                const auto* orig_piece = original.piece_at(pos);
                const auto* copy_piece = copy.piece_at(pos);
                
                if (orig_piece) {
                    REQUIRE(copy_piece != nullptr);
                    REQUIRE(copy_piece->getType() == orig_piece->getType());
                    REQUIRE(copy_piece->getColor() == orig_piece->getColor());
                } else {
                    REQUIRE(copy_piece == nullptr);
                }
            }
        }
    }
    
    SECTION("Assignment operator") {
        Board assigned;
        assigned = original;
        
        for (int r = 0; r < 8; ++r) {
            for (int c = 0; c < 8; ++c) {
                Position pos(r, c);
                const auto* orig_piece = original.piece_at(pos);
                const auto* assigned_piece = assigned.piece_at(pos);
                
                if (orig_piece) {
                    REQUIRE(assigned_piece != nullptr);
                    REQUIRE(assigned_piece->getType() == orig_piece->getType());
                    REQUIRE(assigned_piece->getColor() == orig_piece->getColor());
                } else {
                    REQUIRE(assigned_piece == nullptr);
                }
            }
        }
    }
    
    SECTION("Clone method") {
        Board cloned = original.clone();
        
        for (int r = 0; r < 8; ++r) {
            for (int c = 0; c < 8; ++c) {
                Position pos(r, c);
                const auto* orig_piece = original.piece_at(pos);
                const auto* cloned_piece = cloned.piece_at(pos);
                
                if (orig_piece) {
                    REQUIRE(cloned_piece != nullptr);
                    REQUIRE(cloned_piece->getType() == orig_piece->getType());
                    REQUIRE(cloned_piece->getColor() == orig_piece->getColor());
                } else {
                    REQUIRE(cloned_piece == nullptr);
                }
            }
        }
    }
}

TEST_CASE("Initial Position Setup", "[board]") {
    Board board;
    board.setup_initial_position();
    
    SECTION("Pawns are in correct positions") {
        for (int c = 0; c < 8; ++c) {
            Position whitePawn(1, c);
            Position blackPawn(6, c);
            
            const auto* wp = board.piece_at(whitePawn);
            const auto* bp = board.piece_at(blackPawn);
            
            REQUIRE(wp != nullptr);
            REQUIRE(bp != nullptr);
            REQUIRE(wp->getType() == PieceType::Pawn);
            REQUIRE(bp->getType() == PieceType::Pawn);
            REQUIRE(wp->getColor() == Color::White);
            REQUIRE(bp->getColor() == Color::Black);
        }
    }
    
    SECTION("Back rank pieces") {
        // White back rank
        REQUIRE(board.piece_at(Position(0, 0))->getType() == PieceType::Rook);
        REQUIRE(board.piece_at(Position(0, 1))->getType() == PieceType::Knight);
        REQUIRE(board.piece_at(Position(0, 2))->getType() == PieceType::Bishop);
        REQUIRE(board.piece_at(Position(0, 3))->getType() == PieceType::Queen);
        REQUIRE(board.piece_at(Position(0, 4))->getType() == PieceType::King);
        REQUIRE(board.piece_at(Position(0, 5))->getType() == PieceType::Bishop);
        REQUIRE(board.piece_at(Position(0, 6))->getType() == PieceType::Knight);
        REQUIRE(board.piece_at(Position(0, 7))->getType() == PieceType::Rook);
        
        // Black back rank
        REQUIRE(board.piece_at(Position(7, 0))->getType() == PieceType::Rook);
        REQUIRE(board.piece_at(Position(7, 1))->getType() == PieceType::Knight);
        REQUIRE(board.piece_at(Position(7, 2))->getType() == PieceType::Bishop);
        REQUIRE(board.piece_at(Position(7, 3))->getType() == PieceType::Queen);
        REQUIRE(board.piece_at(Position(7, 4))->getType() == PieceType::King);
        REQUIRE(board.piece_at(Position(7, 5))->getType() == PieceType::Bishop);
        REQUIRE(board.piece_at(Position(7, 6))->getType() == PieceType::Knight);
        REQUIRE(board.piece_at(Position(7, 7))->getType() == PieceType::Rook);
    }
    
    SECTION("All pieces have correct colors") {
        for (int r = 0; r < 2; ++r) {
            for (int c = 0; c < 8; ++c) {
                Position pos(r, c);
                const auto* piece = board.piece_at(pos);
                REQUIRE(piece != nullptr);
                REQUIRE(piece->getColor() == Color::White);
            }
        }
        
        for (int r = 6; r < 8; ++r) {
            for (int c = 0; c < 8; ++c) {
                Position pos(r, c);
                const auto* piece = board.piece_at(pos);
                REQUIRE(piece != nullptr);
                REQUIRE(piece->getColor() == Color::Black);
            }
        }
    }
}

TEST_CASE("Check Detection", "[board]") {
    Board board;
    
    SECTION("No check in initial position") {
        board.setup_initial_position();
        REQUIRE_FALSE(board.is_check(Color::White));
        REQUIRE_FALSE(board.is_check(Color::Black));
    }
    
    SECTION("Simple check scenario") {
        // Place white king
        board.set_piece(Position(4, 4), std::make_unique<King>(Color::White));
        
        // Place black rook attacking the king
        board.set_piece(Position(4, 0), std::make_unique<Rook>(Color::Black));
        
        REQUIRE(board.is_check(Color::White));
        REQUIRE_FALSE(board.is_check(Color::Black));
    }
    
    SECTION("No check when pieces are blocked") {
        // Place white king
        board.set_piece(Position(4, 4), std::make_unique<King>(Color::White));
        
        // Place black rook but block it with white pawn
        board.set_piece(Position(4, 0), std::make_unique<Rook>(Color::Black));
        board.set_piece(Position(4, 2), std::make_unique<Pawn>(Color::White));
        
        REQUIRE_FALSE(board.is_check(Color::White));
    }
}
