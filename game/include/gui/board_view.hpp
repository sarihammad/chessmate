#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <domain/board.hpp>
#include <domain/piece.hpp>

namespace chess {

// BoardView handles rendering the chess board and pieces, and board-related input
class BoardView {
public:
    BoardView(const sf::Font& font, float tileSize = 100.f);
    void draw(sf::RenderWindow& window, const Board& board) const;
    // Returns the board position (row, col) for a given mouse position, or {-1, -1} if out of bounds
    std::pair<int, int> screenToBoard(const sf::Vector2i& mousePos) const;
    // Returns the top-left pixel position for a given board square
    sf::Vector2f boardToScreen(int row, int col) const;
    // Get the image path for a piece
    std::string getImagePath(const Piece& piece) const;
    // Optionally, handle board-specific input (e.g., highlight, drag, etc.)
    // void handleInput(...);

private:
    float tileSize_;
    sf::Font font_;
    // Optionally, cache piece textures/sprites here
};

} // namespace chess 