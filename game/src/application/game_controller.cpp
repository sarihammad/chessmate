#include "application/game_controller.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <gui/widgets/Button.hpp>
#include <gui/board_view.hpp>
#include <domain/game.hpp>
#include <domain/board.hpp>
#include <domain/move.hpp>
#include <domain/player.hpp>
#include <infrastructure/players/human_player.hpp>
#include <infrastructure/players/network_player.hpp>
#include <infrastructure/players/ai_player.hpp>
#include <vector>
#include <memory>
#include <nlohmann/json.hpp>

namespace chess {

GameController::GameController() {}

void GameController::createLocalPlayers() {
    // Instantiate two human players and set them in Game
    auto white = std::make_unique<HumanPlayer>(Color::White);
    auto black = std::make_unique<HumanPlayer>(Color::Black);
    game_->setWhitePlayer(std::move(white));
    game_->setBlackPlayer(std::move(black));
}

void GameController::createOnlinePlayers(WebSocketClient& wsClient) {
    // Instantiate a human player and a network player and set them in Game
    auto white = std::make_unique<HumanPlayer>(Color::White);
    auto black = std::make_unique<NetworkPlayer>(Color::Black, wsClient);
    game_->setWhitePlayer(std::move(white));
    game_->setBlackPlayer(std::move(black));
}

void GameController::createAIPlayers() {
    // Instantiate a human player and an AI player and set them in Game
    std::cout << "Creating AI players..." << std::endl;
    auto white = std::make_unique<HumanPlayer>(Color::White);
    auto black = std::make_unique<AIPlayer>(Color::Black);
    std::cout << "DEBUG: Created Human player (White)" << std::endl;
    std::cout << "DEBUG: Created AI player (Black)" << std::endl;
    game_->setWhitePlayer(std::move(white));
    game_->setBlackPlayer(std::move(black));
    std::cout << "DEBUG: Players set in game" << std::endl;
}

void GameController::showGameOver(sf::RenderWindow& window, const std::string& message) {
    state_ = AppState::Menu;
}

void GameController::startLocalGame() {
    std::cout << "Starting local game..." << std::endl;
    state_ = AppState::LocalGame;
    game_ = std::make_unique<Game>();
    createLocalPlayers();
    std::cout << "Game initialized with players" << std::endl;
    if (!boardView_) {
        sf::Font font;
        if (!font.openFromFile("assets/fonts/Arial.ttf")) {
            std::cerr << "Failed to load font: assets/fonts/Arial.ttf" << std::endl;
            return;
        }
        boardView_ = std::make_unique<BoardView>(font, 100.f);
    }
    std::cout << "Local game started successfully" << std::endl;
}

void GameController::startOnlineGame() {
    state_ = AppState::OnlineGame;
    game_ = std::make_unique<Game>();
    if (!boardView_) {
        sf::Font font;
        if (!font.openFromFile("assets/fonts/Arial.ttf")) {
            std::cerr << "Failed to load font: assets/fonts/Arial.ttf" << std::endl;
            return;
        }
        boardView_ = std::make_unique<BoardView>(font, 100.f);
    }

    // Set up WebSocket client
    std::string serverUri = "ws://localhost:8080/game"; // Or configurable
    wsClient_ = std::make_unique<WebSocketClient>();
    wsClient_->connect(serverUri);

    // Create players
    createOnlinePlayers(*wsClient_);
}

void GameController::startAIGame() {
    std::cout << "Starting AI game..." << std::endl;
    state_ = AppState::AIGame;
    game_ = std::make_unique<Game>();
    createAIPlayers();
    std::cout << "AI game initialized with players" << std::endl;
    std::cout << "DEBUG: Initial turn: " << (game_->getCurrentTurn() == Color::White ? "White" : "Black") << std::endl;
    
    // Test White player
    std::cout << "DEBUG: White player type: " << (dynamic_cast<HumanPlayer*>(game_->getCurrentPlayer()) ? "Human" : "AI") << std::endl;
    
    if (!boardView_) {
        sf::Font font;
        if (!font.openFromFile("assets/fonts/Arial.ttf")) {
            std::cerr << "Failed to load font: assets/fonts/Arial.ttf" << std::endl;
            return;
        }
        boardView_ = std::make_unique<BoardView>(font, 100.f);
    }
    std::cout << "AI game started successfully" << std::endl;
}

void GameController::run() {
    sf::RenderWindow window(sf::VideoMode({800, 800}), "Chessmate");
    window.setFramerateLimit(60);

    // Load font for UI
    sf::Font font;
    if (!font.openFromFile("assets/fonts/Arial.ttf")) {
        std::cerr << "Failed to load font: assets/fonts/Arial.ttf" << std::endl;
        return;
    }

    // Create menu buttons
    std::vector<Button> buttons;
    std::vector<std::string> labels = {"Local Game", "Online Game", "AI Game", "Quit"};
    float btnWidth = 300, btnHeight = 60, spacing = 20;
    float startY = 300;
    for (int i = 0; i < 4; ++i) {
        buttons.emplace_back(font, labels[i], 400 - btnWidth/2, startY + i * (btnHeight + spacing), btnWidth, btnHeight);
    }

    // Set button actions
    buttons[0].setOnClick([&]() { startLocalGame(); });
    buttons[1].setOnClick([&]() { startOnlineGame(); });
    buttons[2].setOnClick([&]() { startAIGame(); });
    buttons[3].setOnClick([&]() { window.close(); });

    // Game state variables
    std::pair<int, int> selected = {-1, -1};
    Position from, to;

    // Main game loop
    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
                return;
            }
            if (event->is<sf::Event::MouseButtonPressed>()) {
                const auto* mouseEvent = event->getIf<sf::Event::MouseButtonPressed>();
                if (mouseEvent->button == sf::Mouse::Button::Left) {
                    sf::Vector2i mousePos = {mouseEvent->position.x, mouseEvent->position.y};
                    if (state_ == AppState::Menu) {
                        for (auto& btn : buttons) {
                            if (btn.handleClick(mousePos)) {
                                break;
                            }
                        }
                    } else if (state_ == AppState::LocalGame || state_ == AppState::AIGame) {
                        // Handle game board clicks
                        auto [row, col] = boardView_->screenToBoard({mouseEvent->position.x, mouseEvent->position.y});
                        if (row >= 0 && col >= 0) {
                            if (selected.first == -1) {
                                // Selecting a piece
                                auto piece = game_->getBoard().getPieceAt({row, col});
                                if (piece && piece->getColor() == game_->getCurrentTurn()) {
                                    // In AI game, only allow selecting White pieces (human player)
                                    if (state_ == AppState::AIGame && piece->getColor() != Color::White) {
                                        std::cout << "DEBUG: Cannot select Black piece in AI game" << std::endl;
                                        continue;
                                    }
                                    selected = {row, col};
                                    from = Position(row, col);
                                    std::cout << "Selected piece at (" << row << ", " << col << ")" << std::endl;
                                }
                            } else {
                                // Moving to a square
                                to = Position(row, col);
                                std::cout << "Attempting move from (" << from.row << ", " << from.col << ") to (" << to.row << ", " << to.col << ")" << std::endl;
                                
                                // In AI game, only allow moving White pieces (human player)
                                auto movingPiece = game_->getBoard().getPieceAt(from);
                                if (state_ == AppState::AIGame && movingPiece && movingPiece->getColor() != Color::White) {
                                    std::cout << "DEBUG: Cannot move Black pieces in AI game" << std::endl;
                                    selected = {-1, -1};
                                    continue;
                                }
                                
                                // Use the player system to make the move
                                Player* currentPlayer = game_->getCurrentPlayer();
                                if (currentPlayer && currentPlayer->makeMove(game_->getBoard(), from, to)) {
                                    game_->playTurn(from, to);
                                    std::cout << "Move executed successfully" << std::endl;
                                    // Note: humanJustMoved will be set in the AI logic section
                                } else {
                                    std::cout << "Invalid move" << std::endl;
                                }
                                selected = {-1, -1};
                            }
                        }
                    } else if (state_ == AppState::OnlineGame) {
                        // Handle online game clicks (only for local player)
                        if (game_->getCurrentTurn() == Color::White) {
                            auto [row, col] = boardView_->screenToBoard({mouseEvent->position.x, mouseEvent->position.y});
                            if (row >= 0 && col >= 0) {
                                if (selected.first == -1) {
                                    if (game_->getBoard().getPieceAt({row, col}) &&
                                        game_->getBoard().getPieceAt({row, col})->getColor() == Color::White) {
                                        selected = {row, col};
                                        from = Position(row, col);
                                    }
                                } else {
                                    to = Position(row, col);
                                    Player* currentPlayer = game_->getCurrentPlayer();
                                    if (currentPlayer && currentPlayer->makeMove(game_->getBoard(), from, to)) {
                                        game_->playTurn(from, to);
                                        // Send move to server
                                        if (wsClient_) {
                                            nlohmann::json msg = { {"type", "move"}, {"from", { {"row", from.row}, {"col", from.col} }}, {"to", { {"row", to.row}, {"col", to.col} }} };
                                            wsClient_->sendMessage(msg);
                                        }
                                    }
                                    selected = {-1, -1};
                                }
                            }
                        }
                    }
                }
            }
        }

        // Update button hover states for menu
        if (state_ == AppState::Menu) {
            sf::Vector2i mousePos = {sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y};
            for (auto& btn : buttons) {
                btn.update(mousePos);
            }
        }

        // Handle network player moves for online game
        if (state_ == AppState::OnlineGame && game_->getCurrentTurn() == Color::Black) {
            Position netFrom, netTo;
            Player* currentPlayer = game_->getCurrentPlayer();
            if (currentPlayer && currentPlayer->makeMove(game_->getBoard(), netFrom, netTo)) {
                game_->playTurn(netFrom, netTo);
            }
        }

        // Handle AI moves for AI game
        static bool aiMoveMadeThisTurn = false;
        static Color lastTurn = Color::White;
        static bool humanJustMoved = false;
        
        // Detect if a human move was just made (turn changed from White to Black)
        if (state_ == AppState::AIGame && lastTurn == Color::White && game_->getCurrentTurn() == Color::Black) {
            humanJustMoved = true;
        }
        
        if (state_ == AppState::AIGame && !game_->isGameOver() && game_->getCurrentTurn() == Color::Black) {
            // Reset flag if turn changed
            if (lastTurn != Color::Black) {
                aiMoveMadeThisTurn = false;
                lastTurn = Color::Black;
            }
            
            // Only make AI move if a human move was just made (not if Black was manually moved)
            if (!humanJustMoved) {
                std::cout << "DEBUG: Skipping AI move - no human move detected" << std::endl;
            } else {
                Player* currentPlayer = game_->getCurrentPlayer();
                std::cout << "DEBUG: AI Game - It's Black's turn (AI should move)" << std::endl;
                std::cout << "DEBUG: AI Game - Current player type: " << (dynamic_cast<AIPlayer*>(currentPlayer) ? "AI" : "Human") << std::endl;
                std::cout << "DEBUG: AI Game - AI move already made this turn: " << (aiMoveMadeThisTurn ? "Yes" : "No") << std::endl;
                
                if (currentPlayer && dynamic_cast<AIPlayer*>(currentPlayer) && !aiMoveMadeThisTurn) {
                    // It's the AI's turn, make a move
                    std::cout << "DEBUG: AI's turn, calculating move..." << std::endl;
                    Position aiFrom, aiTo;
                    if (currentPlayer->makeMove(game_->getBoard(), aiFrom, aiTo)) {
                        std::cout << "AI making move from (" << aiFrom.row << ", " << aiFrom.col 
                                  << ") to (" << aiTo.row << ", " << aiTo.col << ")" << std::endl;
                        game_->playTurn(aiFrom, aiTo);
                        aiMoveMadeThisTurn = true;
                        
                        // Add a small delay to make AI moves visible
                        sf::sleep(sf::milliseconds(500));
                    } else {
                        std::cout << "DEBUG: AI failed to make a move" << std::endl;
                    }
                } else if (aiMoveMadeThisTurn) {
                    std::cout << "DEBUG: AI already made its move this turn" << std::endl;
                } else {
                    std::cout << "DEBUG: Current player is not AI or is null" << std::endl;
                }
            }
            humanJustMoved = false; // Reset the flag
        } else {
            // Reset flag when not in AI game or when it's White's turn
            if (lastTurn != Color::White) {
                aiMoveMadeThisTurn = false;
                lastTurn = Color::White;
            }
        }

        // Render based on current state
        window.clear();
        
        if (state_ == AppState::Menu) {
            // Render menu
            for (const auto& btn : buttons) {
                btn.draw(window);
            }
        } else {
            // Render game board
            if (boardView_ && game_) {
                boardView_->draw(window, game_->getBoard());
                
                // Draw selection highlight
                if (selected.first != -1) {
                    sf::RectangleShape highlight(sf::Vector2f(100.f, 100.f));
                    highlight.setPosition(boardView_->boardToScreen(selected.first, selected.second));
                    highlight.setFillColor(sf::Color(0, 255, 0, 80));
                    window.draw(highlight);
                }
            }
        }
        
        window.display();

        // Check for game over
        if (game_ && game_->isGameOver()) {
            showGameOver(window, "Game Over");
            state_ = AppState::Menu;
            selected = {-1, -1};
        }
    }
}

}