#pragma once
#include <SFML/Graphics.hpp>
#include <functional>
#include <string>

namespace chess {

class Button {
public:
    Button(const sf::Font& font, const std::string& text, float x, float y, float width, float height);
    
    bool handleClick(const sf::Vector2i& mousePos);
    void update(const sf::Vector2i& mousePos);
    void draw(sf::RenderWindow& window) const;
    void setOnClick(std::function<void()> callback);

private:
    sf::RectangleShape rect;
    sf::Text label;
    std::function<void()> onClick;
};

} // namespace chess 