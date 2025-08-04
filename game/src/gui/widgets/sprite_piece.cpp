#include "gui/widgets/sprite_piece.hpp"
#include "domain/piece.hpp"
#include "gui/texture_manager.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>

namespace chess {

SpritePiece::SpritePiece(std::shared_ptr<Piece> piece, const std::string& imagePath, float x, float y)
    : piece(piece) {
    // Load texture and create sprite
    try {
        sf::Texture& texture = TextureManager::getTexture(imagePath);
        sprite.emplace(texture);
        sprite->setPosition(sf::Vector2f(x, y));
    } catch (const std::exception& e) {
        // Handle texture loading failure
        std::cerr << "Failed to load sprite texture: " << imagePath << std::endl;
    }
}

void SpritePiece::setPosition(float x, float y) {
    if (sprite) sprite->setPosition(sf::Vector2f(x, y));
}

void SpritePiece::draw(sf::RenderWindow& window) const {
    if (sprite) window.draw(*sprite);
}

} // namespace chess