#include <SFML/Graphics.hpp>
#include <iostream>
#include "gui/gui.hpp"

int main() {
    sf::RenderWindow window(sf::VideoMode({800, 800}), "Chessmate");
    window.setFramerateLimit(60);
    
    while (window.isOpen()) {
        try {
            chess::runMainMenu(window, "ws://localhost:8080/game");
        } catch (const std::runtime_error& e) {
            if (std::string(e.what()) == "Return to menu") {
                // Continue to show main menu again
                continue;
            } else {
                std::cerr << "Fatal error: " << e.what() << std::endl;
                return 1;
            }
        } catch (const std::exception& e) {
            std::cerr << "Fatal error: " << e.what() << std::endl;
            return 1;
        }
    }
    
    return 0;
} 