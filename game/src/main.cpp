#include <SFML/Graphics.hpp>
#include <iostream>
#include "gui/gui.hpp"
#include "infrastructure/config.hpp"

int main(int argc, char* argv[]) {
    // Load configuration
    auto config_opt = chess::load_config("config.json");
    if (!config_opt) {
        std::cout << "Using default configuration\n";
        config_opt = chess::load_default_config();
    }
    
    const auto& config = *config_opt;
    
    // Create window with config settings
    sf::RenderWindow window(
        sf::VideoMode({static_cast<unsigned>(config.window_width), 
                      static_cast<unsigned>(config.window_height)}), 
        "Chessmate"
    );
    
    window.setFramerateLimit(static_cast<unsigned>(config.target_fps));
    window.setVerticalSyncEnabled(config.vsync);
    
    if (config.debug_mode) {
        std::cout << "Debug mode enabled\n";
        std::cout << "WebSocket URL: " << config.ws_url << "\n";
        std::cout << "Window size: " << config.window_width << "x" << config.window_height << "\n";
        std::cout << "AI depth: " << config.ai_depth << "\n";
    }
    
    while (window.isOpen()) {
        try {
            chess::runMainMenu(window, config.ws_url);
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