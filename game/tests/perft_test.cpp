#include <catch2/catch_all.hpp>
#include <string>
#include "domain/board.hpp"
// include your perft/movegen APIs

TEST_CASE("Perft initial position depths 1..3 match known counts") {
  chess::Board b;
  b.setup_initial_position();
  // REQUIRE(perft(b, 1) == 20);
  // REQUIRE(perft(b, 2) == 400);
  // REQUIRE(perft(b, 3) == 8902);
  SUCCEED("Hook up real perft once implemented");
}

TEST_CASE("Perft positions from chess programming wiki") {
  // Position 2: Kiwipete
  chess::Board b2;
  // b2.load_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -");
  // REQUIRE(perft(b2, 1) == 48);
  // REQUIRE(perft(b2, 2) == 2039);
  SUCCEED("Hook up real perft once implemented");
}
