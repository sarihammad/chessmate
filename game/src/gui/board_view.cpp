#include "gui/board_view.hpp"
#include <SFML/Graphics.hpp>
#include <domain/board.hpp>
#include <domain/piece.hpp>
#include <array>
#include <iostream>
#include "gui/widgets/sprite_piece.hpp"
#include "gui/texture_manager.hpp"

namespace chess {

BoardView::BoardView(const sf::Font& font, float tileSize)
    : tileSize_(tileSize), font_(font) {}

void BoardView::draw(sf::RenderWindow& window, const Board& board) const {
    static int drawCount = 0;
    drawCount++;
    if (drawCount == 1) {
        std::cout << "BoardView::draw called for the first time" << std::endl;
    }
    
    // Draw chessboard background
    static sf::Texture boardTexture;
    static bool boardTextureLoaded = false;
    
    if (!boardTextureLoaded) {
        if (boardTexture.loadFromFile("assets/images/board.png")) {
            boardTextureLoaded = true;
            std::cout << "Board texture loaded successfully" << std::endl;
        } else {
            std::cout << "Failed to load board texture, using fallback" << std::endl;
        }
    }
    
    if (boardTextureLoaded) {
        sf::Sprite boardSprite(boardTexture);
        boardSprite.setScale(sf::Vector2f(tileSize_ * 8 / boardTexture.getSize().x, tileSize_ * 8 / boardTexture.getSize().y));
        window.draw(boardSprite);
    } else {
        // Fallback: Draw chessboard squares
        for (int row = 0; row < 8; ++row) {
            for (int col = 0; col < 8; ++col) {
                sf::RectangleShape square(sf::Vector2f(tileSize_, tileSize_));
                square.setPosition({col * tileSize_, row * tileSize_});
                bool isLight = (row + col) % 2 == 0;
                square.setFillColor(isLight ? sf::Color(240, 217, 181) : sf::Color(181, 136, 99));
                window.draw(square);
            }
        }
    }
    
    // Draw pieces using sprites (flip board so white is at bottom)
    static int pieceCount = 0;
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            const Piece* piece = board.getPieceAt({row, col}).get();
            if (piece) {
                pieceCount++;
                if (drawCount == 1) {
                    std::cout << "Rendering piece " << piece->symbol() << " at (" << row << ", " << col << ")" << std::endl;
                }
                std::string imagePath = getImagePath(*piece);
                try {
                    sf::Sprite pieceSprite(TextureManager::getTexture(imagePath));
                    // Flip the board: row 0 becomes row 7, row 1 becomes row 6, etc.
                    int displayRow = 7 - row;
                    pieceSprite.setPosition(sf::Vector2f(col * tileSize_, displayRow * tileSize_));
                    pieceSprite.setScale(sf::Vector2f(tileSize_ / pieceSprite.getTexture().getSize().x, 
                                                    tileSize_ / pieceSprite.getTexture().getSize().y));
                    window.draw(pieceSprite);
                } catch (const std::runtime_error& e) {
                    std::cout << "Failed to load piece texture: " << imagePath << " - " << e.what() << std::endl;
                    // Fallback to text if image loading fails
                    sf::Text pieceText(font_);
                    pieceText.setCharacterSize(static_cast<unsigned int>(tileSize_ * 0.7f));
                    pieceText.setFillColor(piece->getColor() == Color::White ? sf::Color::White : sf::Color::Black);
                    pieceText.setString(std::string(1, piece->symbol()));
                    // Flip the board for text too
                    int displayRow = 7 - row;
                    pieceText.setPosition(sf::Vector2f(
                        col * tileSize_ + tileSize_ * 0.2f,
                        displayRow * tileSize_ + tileSize_ * 0.05f
                    ));
                    window.draw(pieceText);
                }
            }
        }
    }
    if (drawCount == 1) {
        std::cout << "Total pieces rendered: " << pieceCount << std::endl;
    }
}

std::pair<int, int> BoardView::screenToBoard(const sf::Vector2i& mousePos) const {
    int col = mousePos.x / static_cast<int>(tileSize_);
    int displayRow = mousePos.y / static_cast<int>(tileSize_);
    if (displayRow < 0 || displayRow >= 8 || col < 0 || col >= 8) return {-1, -1};
    // Flip the row back to board coordinates
    int row = 7 - displayRow;
    return {row, col};
}

sf::Vector2f BoardView::boardToScreen(int row, int col) const {
    // Flip the row for display
    int displayRow = 7 - row;
    return sf::Vector2f(col * tileSize_, displayRow * tileSize_);
}

std::string BoardView::getImagePath(const Piece& piece) const {
    std::string color = (piece.getColor() == Color::White) ? "white" : "black";
    std::string type;
    switch (piece.getType()) {
        case PieceType::Pawn: type = "pawn"; break;
        case PieceType::Rook: type = "rook"; break;
        case PieceType::Knight: type = "knight"; break;
        case PieceType::Bishop: type = "bishop"; break;
        case PieceType::Queen: type = "queen"; break;
        case PieceType::King: type = "king"; break;
    }
    return "assets/images/" + color + "_" + type + ".png";
}

} // namespace chess 