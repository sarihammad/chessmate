#pragma once
#include <SFML/Graphics.hpp>
#include <functional>
#include <memory>
#include <string>
#include <optional>

namespace chess {

class Piece;

class SpritePiece {
public:
    SpritePiece(std::shared_ptr<Piece> piece, const std::string& imagePath, float x, float y);
    
    void setPosition(float x, float y);
    void draw(sf::RenderWindow& window) const;
    
    std::optional<sf::Sprite> sprite;
    std::shared_ptr<Piece> piece;
};

} // namespace chess
