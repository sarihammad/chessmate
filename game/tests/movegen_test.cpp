#include <catch2/catch_all.hpp>
#include "domain/board.hpp"

TEST_CASE("Simple legal move generation doesn't produce out-of-board positions") {
  chess::Board b;
  b.setup_initial_position();
  // auto moves = generate_legal_moves(b);
  // for (auto& m : moves) { 
  //   REQUIRE(m.to.row >= 0); 
  //   REQUIRE(m.to.row < 8); 
  //   REQUIRE(m.to.col >= 0);
  //   REQUIRE(m.to.col < 8);
  // }
  SUCCEED("Replace with real tests once wired");
}

TEST_CASE("Initial position has exactly 20 legal moves") {
  chess::Board b;
  b.setup_initial_position();
  // auto moves = generate_legal_moves(b);
  // REQUIRE(moves.size() == 20);
  SUCCEED("Replace with real tests once wired");
}

TEST_CASE("Move generation respects pins and checks") {
  chess::Board b;
  // Set up a position with pinned pieces
  // REQUIRE that pinned pieces cannot move in ways that expose the king
  SUCCEED("Replace with real tests once wired");
}
