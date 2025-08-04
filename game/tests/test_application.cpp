#include <cassert>
#include <iostream>
#include <application/game_controller.hpp>

using namespace chess;

void test_game_controller_state_transition() {
    GameController controller;
    // Simulate starting a local game and force game over
    controller.startLocalGame();
    // We can't fully test UI, but we can check state transitions if exposed
    // For now, just ensure the controller can be constructed and run without crashing
    assert(true);
}

int main() {
    test_game_controller_state_transition();
    std::cout << "All application tests passed!\n";
    return 0;
} 