#include <iostream>
#include <optional>
#include <string>
#include "infrastructure/config.hpp"
#include <SFML/Graphics.hpp>
#include "gui/gui.hpp"

int main(int argc, char** argv) {
  std::optional<std::string> cfgPath;
  for (int i = 1; i < argc; ++i) {
    std::string_view arg{argv[i]};
    if (arg == "--config" && i + 1 < argc) cfgPath = std::string{argv[++i]};
  }

  auto cfg = chess::load_config(cfgPath);

  // Create window from cfg.ui
  sf::RenderWindow window(
      sf::VideoMode({static_cast<unsigned>(cfg.ui.window_width), 
                    static_cast<unsigned>(cfg.ui.window_height)}), 
      "Chessmate"
  );
  
  window.setVerticalSyncEnabled(cfg.ui.vsync);

  // Explicit state machine enum
  chess::AppState state = chess::AppState::Menu;

  while (state != chess::AppState::Quit && window.isOpen()) {
    try {
      switch (state) {
        case chess::AppState::Menu: 
          state = chess::runMainMenu(window, cfg); 
          break;
        case chess::AppState::Game: 
          state = chess::runGame(window, cfg); 
          break;
        case chess::AppState::Quit: 
          break;
      }
    } catch (const std::runtime_error& e) {
      if (std::string(e.what()) == "Return to menu") {
        state = chess::AppState::Menu;
      } else {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        state = chess::AppState::Quit;
      }
    } catch (const std::exception& e) {
      std::cerr << "Fatal error: " << e.what() << std::endl;
      state = chess::AppState::Quit;
    }
  }
  
  return 0;
}