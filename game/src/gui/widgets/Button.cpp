#include "gui/widgets/Button.hpp"
#include <SFML/Graphics.hpp>

namespace chess {

Button::Button(const sf::Font& font, const std::string& text, float x, float y, float width, float height)
    : rect(sf::Vector2f(width, height)), label(font, text, 24) {
    rect.setPosition(sf::Vector2f(x, y));
    rect.setFillColor(sf::Color(50, 50, 50, 200));
    rect.setOutlineThickness(2);
    rect.setOutlineColor(sf::Color(0, 200, 0));
    
    label.setFillColor(sf::Color(200, 255, 200));
    label.setPosition(sf::Vector2f(x + 10, y + 5));
}

bool Button::handleClick(const sf::Vector2i& mousePos) {
    if (rect.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos))) {
        if (onClick) onClick();
        return true;
    }
    return false;
}

void Button::update(const sf::Vector2i& mousePos) {
    bool isHovered = rect.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos));
    rect.setFillColor(isHovered ? sf::Color(0, 200, 0, 200) : sf::Color(50, 50, 50, 200));
    label.setFillColor(isHovered ? sf::Color::White : sf::Color(200, 255, 200));
}

void Button::draw(sf::RenderWindow& window) const {
    window.draw(rect);
    window.draw(label);
}

void Button::setOnClick(std::function<void()> callback) {
    onClick = std::move(callback);
}

} // namespace chess 