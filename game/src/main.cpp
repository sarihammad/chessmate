#include <iostream>
#include <optional>
#include <string>
#include "infrastructure/config.hpp"
#include <spdlog/spdlog.h>
#include <SFML/Graphics.hpp>
#include "gui/gui.hpp"

int main(int argc, char** argv) {
  std::optional<std::string> cfgPath;
  for (int i = 1; i < argc; ++i) {
    std::string_view arg{argv[i]};
    if (arg == "--config" && i + 1 < argc) cfgPath = std::string{argv[++i]};
  }

  auto cfg = cm::load_config(cfgPath);
  spdlog::info("Chessmate starting. ws_url={}, reconnect_ms={}", cfg.network.ws_url, cfg.network.reconnect_ms);

  // Create window with config settings
  sf::RenderWindow window(
      sf::VideoMode({static_cast<unsigned>(cfg.ui.window_width), 
                    static_cast<unsigned>(cfg.ui.window_height)}), 
      "Chessmate"
  );
  
  window.setVerticalSyncEnabled(cfg.ui.vsync);

  // Example explicit state loop instead of throwing strings
  enum class AppState { Menu, Game, Quit };
  AppState state = AppState::Menu;

  while (state != AppState::Quit && window.isOpen()) {
    switch (state) {
      case AppState::Menu:
        // TODO: Change runMainMenu to return AppState and take Config
        try {
          chess::runMainMenu(window, cfg.network.ws_url);
          state = AppState::Quit; // For now, quit after menu
        } catch (const std::runtime_error& e) {
          if (std::string(e.what()) == "Return to menu") {
            state = AppState::Menu;
          } else {
            spdlog::error("Fatal error: {}", e.what());
            state = AppState::Quit;
          }
        } catch (const std::exception& e) {
          spdlog::error("Fatal error: {}", e.what());
          state = AppState::Quit;
        }
        break;
      case AppState::Game:
        // TODO: Implement runGame that returns AppState
        state = AppState::Menu; // For now, return to menu
        break;
      case AppState::Quit:
        break;
    }
  }
  spdlog::info("Chessmate exiting normally.");
  return 0;
}