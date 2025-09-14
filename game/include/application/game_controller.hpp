#pragma once
#include <memory>
#include <SFML/Graphics.hpp>
#include <gui/board_view.hpp>
#include <domain/game.hpp>
#include <infrastructure/network/websocket_client.hpp>

namespace chess {

class Player;

// orchestrates the game loop, state transitions, and delegates to domain logic and UI
class GameController {
public:
    GameController();
    void run();
    void handleEvent(const sf::Event& event);
    void update();
    void render(sf::RenderWindow& window);

private:
    enum class AppState { Menu, LocalGame, OnlineGame, AIGame };
    AppState state_ = AppState::Menu;

    void startLocalGame();
    void startOnlineGame();
    void startAIGame();
    void showGameOver(sf::RenderWindow& window, const std::string& message);

    // Player management
    void createLocalPlayers();
    void createOnlinePlayers(cm::WebSocketClient& wsClient);
    void createAIPlayers();

    std::unique_ptr<Game> game_;
    std::unique_ptr<BoardView> boardView_;
    std::unique_ptr<cm::WebSocketClient> wsClient_;
    
};

}