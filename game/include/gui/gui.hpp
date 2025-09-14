#pragma once

#include <SFML/Graphics.hpp>
#include "infrastructure/network/websocket_client.hpp"

namespace chess {
    void runMainMenu(sf::RenderWindow& window, const std::string& serverUri);
    void runGUI(sf::RenderWindow& window, cm::WebSocketClient* wsClient = nullptr);
}
