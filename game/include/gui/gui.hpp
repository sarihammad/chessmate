#pragma once

#include <SFML/Graphics.hpp>
#include "infrastructure/network/websocket_client.hpp"
#include "infrastructure/config.hpp"

namespace chess {

enum class AppState { Menu, Game, Quit };

AppState runMainMenu(sf::RenderWindow& window, const Config& cfg);
AppState runGame(sf::RenderWindow& window, const Config& cfg);

}
