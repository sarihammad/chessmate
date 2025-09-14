#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <map>
#include <optional>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <nlohmann/json.hpp>
#include "domain/game.hpp"
#include "gui/widgets/sprite_piece.hpp"
#include "domain/pieces/rook.hpp"
#include "domain/pieces/bishop.hpp"
#include "domain/pieces/knight.hpp"
#include "domain/pieces/queen.hpp"
#include "infrastructure/network/websocket_client.hpp"
#include "infrastructure/players/network_player.hpp"
#include "infrastructure/config.hpp"
#include "gui/gui.hpp"
#include <vector>
#include <functional>

namespace chess {

constexpr int TILE_SIZE = 100;

struct Button {
    sf::RectangleShape rect;
    sf::Text label;
    std::function<void()> onClick;
    bool hovered = false;
};

std::string getImagePath(const Piece& piece) {
    std::string color = (piece.getColor() == Color::White) ? "white" : "black";
    std::string type;
    switch (piece.getType()) {
        case PieceType::Pawn: type = "pawn"; break;
        case PieceType::Rook: type = "rook"; break;
        case PieceType::Knight: type = "knight"; break;
        case PieceType::Bishop: type = "bishop"; break;
        case PieceType::Queen: type = "queen"; break;
        case PieceType::King: type = "king"; break;
    }
    return "assets/images/" + color + "_" + type + ".png";
}


void runGUI(sf::RenderWindow& window, cm::WebSocketClient* wsClient, bool vsAI, bool humanIsWhite) {
    sf::Font font;
    if (!font.openFromFile("assets/fonts/Arial.ttf")) {
        std::cerr << "Failed to load font for game UI.\n";
        return;
    }
    sf::Texture boardTex;

    if (!boardTex.loadFromFile("assets/images/board.png")) {
        std::cerr << "Failed to load board texture.";
        return;
    }
    sf::Sprite boardSprite(boardTex);

    Game game;
    std::map<Position, SpritePiece> spriteMap;

    std::string assignedColor = humanIsWhite ? "white" : "black";
    bool waitingForMatch = wsClient != nullptr;
    std::string statusMessage = "";
    NetworkPlayer* netPlayer = nullptr;

    // For online games, wait for connection before proceeding
    if (wsClient) {
        statusMessage = "Connecting to server...";
        // Wait for connection with timeout
        int connectionAttempts = 0;
        const int maxAttempts = 50; // 5 seconds at 60fps
        while (!wsClient->is_connected() && connectionAttempts < maxAttempts) {
            // Process events to keep window responsive
            while (const std::optional<sf::Event> event = window.pollEvent()) {
                if (event->is<sf::Event::Closed>()) {
                    window.close();
                    return;
                }
            }
            
            // Show connection status
            window.clear();
            window.draw(boardSprite);
            
            sf::Font statusFont;
            (void)statusFont.openFromFile("assets/fonts/Arial.ttf");
            sf::Text statusText(statusFont, statusMessage, 40);
            statusText.setFillColor(sf::Color::White);
            statusText.setPosition(sf::Vector2f(100.f, 350.f));
            window.draw(statusText);
            window.display();
            
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            connectionAttempts++;
        }
        
        if (!wsClient->is_connected()) {
            statusMessage = "Failed to connect to server";
            // Show error and return to menu
            while (window.isOpen()) {
                while (const std::optional<sf::Event> event = window.pollEvent()) {
                    if (event->is<sf::Event::Closed>()) {
                        window.close();
                        return;
                    }
                }
                
                window.clear();
                window.draw(boardSprite);
                
                sf::Font statusFont;
                (void)statusFont.openFromFile("assets/fonts/Arial.ttf");
                sf::Text statusText(statusFont, statusMessage, 40);
                statusText.setFillColor(sf::Color::Red);
                statusText.setPosition(sf::Vector2f(100.f, 350.f));
                window.draw(statusText);
                window.display();
            }
            return;
        }
        
        statusMessage = "Connected! Waiting for opponent...";
    }

    auto boardToScreen = [&](const Position& pos) -> sf::Vector2f {
        if (assignedColor == "black") {
            return sf::Vector2f(static_cast<float>((7 - pos.col) * TILE_SIZE), static_cast<float>(pos.row * TILE_SIZE));
        }
        return sf::Vector2f(static_cast<float>(pos.col * TILE_SIZE), static_cast<float>((7 - pos.row) * TILE_SIZE));
    };

    auto screenToBoard = [&](const sf::Vector2i& screenPos) -> Position {
        int c = screenPos.x / TILE_SIZE;
        int r = screenPos.y / TILE_SIZE;
        if (assignedColor == "black") {
            return Position(r, 7 - c);
        }
        return Position(7 - r, c);
    };

    auto syncSpriteMap = [&]() {
        spriteMap.clear();
        for (int r = 0; r < 8; ++r) {
            for (int c = 0; c < 8; ++c) {
                Position pos(r, c);
                auto piece = game.getBoard().getPieceAt(pos);
                if (piece) {
                    std::string img = getImagePath(*piece);
                    spriteMap.insert_or_assign(pos, SpritePiece(piece, img, 0, 0));
                    if (spriteMap.at(pos).sprite) spriteMap.at(pos).sprite->setPosition(boardToScreen(pos));
                }
            }
        }
    };

    if (vsAI) {
        if (humanIsWhite) {
            game.setWhitePlayer(std::make_unique<HumanPlayer>(Color::White));
            game.setBlackPlayer(std::make_unique<AIPlayer>(Color::Black));
        } else {
            game.setWhitePlayer(std::make_unique<AIPlayer>(Color::White));
            game.setBlackPlayer(std::make_unique<HumanPlayer>(Color::Black));
        }
    } else if (wsClient) {
        wsClient->setMessageHandler([&](const nlohmann::json& msg) {
            std::cout << "[WS MESSAGE] Received: " << msg.dump() << std::endl;
            if (msg.value("type", "") == "start") {
                if (msg.contains("color")) {
                    assignedColor = msg["color"];
                    waitingForMatch = false;
                    statusMessage = std::string("Matched! You are ") + (assignedColor == "white" ? "White" : "Black");
                    if (assignedColor == "white") {
                        game.setWhitePlayer(std::make_unique<HumanPlayer>(Color::White));
                        auto networkPlayerPtr = std::make_unique<NetworkPlayer>(Color::Black, *wsClient);
                        netPlayer = networkPlayerPtr.get();
                        game.setBlackPlayer(std::move(networkPlayerPtr));
                    } else {
                        auto networkPlayerPtr = std::make_unique<NetworkPlayer>(Color::White, *wsClient);
                        netPlayer = networkPlayerPtr.get();
                        game.setWhitePlayer(std::move(networkPlayerPtr));
                        game.setBlackPlayer(std::make_unique<HumanPlayer>(Color::Black));
                    }
                    std::cout << "[MATCHED] Assigned color: " << assignedColor << std::endl;
                    syncSpriteMap(); 
                } else {
                    std::cerr << "[ERROR] 'start' message missing 'color' field: " << msg.dump() << std::endl;
                }
            } else if (msg.value("type", "") == "opponentMove") {
                Position from(msg["from"]["row"], msg["from"]["col"]);
                Position to(msg["to"]["row"], msg["to"]["col"]);
                std::cout << "[RECV] GUI received opponent move: from (" << from.row << "," << from.col << ") to (" << to.row << "," << to.col << ")" << std::endl;
                game.playTurn(from, to);
                syncSpriteMap();
            } else if (msg.value("type", "") == "gameOver") {
                std::cout << "[INFO] Game over: " << msg.dump() << std::endl;
            }
        });
        statusMessage = "Waiting for opponent...";
        wsClient->sendMessage(nlohmann::json{{"type", "join"}});
    } else {
        game.setWhitePlayer(std::make_unique<HumanPlayer>(Color::White));
        game.setBlackPlayer(std::make_unique<HumanPlayer>(Color::Black));
    }

    game.playSound("game_start");

    bool isDragging = false;
    Position dragStartPos(-1, -1);
    SpritePiece* draggedPiece = nullptr;
    bool isMouseDown = false;
    sf::Vector2i mouseDownPos;
    bool dragConfirmed = false;

    // Move indicators based on your specifications
    sf::CircleShape possibleMoveSprite;
    possibleMoveSprite.setRadius(TILE_SIZE / 5.f);
    possibleMoveSprite.setFillColor(sf::Color(0, 0, 0, 50));
    possibleMoveSprite.setOrigin(sf::Vector2f(TILE_SIZE / 5.f, TILE_SIZE / 5.f));

    sf::RectangleShape possibleCaptureMoveSprite;
    possibleCaptureMoveSprite.setOrigin(sf::Vector2f(TILE_SIZE / 2.f, TILE_SIZE / 2.f));
    possibleCaptureMoveSprite.setSize(sf::Vector2f(TILE_SIZE, TILE_SIZE));
    possibleCaptureMoveSprite.setFillColor(sf::Color(255, 0, 0, 255));

    // bool isWhiteHuman = game.isWhiteHuman();
    // bool isBlackHuman = game.isBlackHuman();

    bool isPromoting = false;
    Position promotionPos(-1, -1);
    Color promotionColor = Color::White;

    sf::Texture whitePromotionTexture;
    if (!whitePromotionTexture.loadFromFile("assets/images/white_promotion.png")) {
        std::cerr << "Failed to load white promotion texture.";
        return;
    }
    sf::Sprite whitePromotionSprite(whitePromotionTexture);

    sf::Texture blackPromotionTexture;
    if (!blackPromotionTexture.loadFromFile("assets/images/black_promotion.png")) {
        std::cerr << "Failed to load black promotion texture.";
        return;
    }
    sf::Sprite blackPromotionSprite(blackPromotionTexture);

    syncSpriteMap();

    Position selected(-1, -1);
    bool hasSelected = false;
    std::vector<Position> validMoves;

    sf::RectangleShape menuBtn(sf::Vector2f(120.f, 45.f));
    menuBtn.setPosition({20.f, 20.f});
    menuBtn.setFillColor(sf::Color(30, 30, 30, 220));
    menuBtn.setOutlineThickness(2.f);
    menuBtn.setOutlineColor(sf::Color(0, 200, 0));
    sf::Text menuLabel(font, "Menu", 28);
    menuLabel.setFillColor(sf::Color(200, 255, 200));
    menuLabel.setStyle(sf::Text::Bold);
    menuLabel.setPosition({menuBtn.getPosition().x + 20.f, menuBtn.getPosition().y + 5.f});

    while (window.isOpen()) {
        while (const std::optional<sf::Event> event = window.pollEvent()) {
            Color localPlayerColor = (assignedColor == "white") ? Color::White : Color::Black;
            bool isMyTurn = !wsClient || (game.getCurrentTurn() == localPlayerColor);
            // For local games, allow moves only for the current turn
            if (!wsClient) {
                isMyTurn = true; // Allow interaction, but check piece color in move logic
            }
            if (waitingForMatch) {
                if (event->is<sf::Event::Closed>()) window.close();
                continue;
            }

            if (event->is<sf::Event::MouseMoved>()) {
                if (!isMyTurn) continue;
                const auto* mouseMoveEvent = event->getIf<sf::Event::MouseMoved>();
                float mx = static_cast<float>(mouseMoveEvent->position.x);
                float my = static_cast<float>(mouseMoveEvent->position.y);

                // Check if we should start dragging (if mouse moved enough and we have a selected piece)
                if (isMouseDown && hasSelected && !isDragging) {
                    int dx = mouseMoveEvent->position.x - mouseDownPos.x;
                    int dy = mouseMoveEvent->position.y - mouseDownPos.y;
                    if (abs(dx) > 10 || abs(dy) > 10) {
                        // Start dragging
                        isDragging = true;
                        dragConfirmed = true;
                        
                        // Calculate valid moves for the dragged piece
                        validMoves.clear();
                        for (int r = 0; r < 8; ++r) {
                            for (int c = 0; c < 8; ++c) {
                                Position to(r, c);
                                if (game.isMoveLegal(dragStartPos, to)) {
                                    validMoves.push_back(to);
                                }
                            }
                        }
                        
                        auto it = spriteMap.find(dragStartPos);
                        if (it != spriteMap.end()) {
                            draggedPiece = &it->second;
                            if (draggedPiece->sprite) {
                                draggedPiece->sprite->setOrigin(sf::Vector2f(TILE_SIZE / 2.f, TILE_SIZE / 2.f));
                                draggedPiece->sprite->setPosition(sf::Vector2f(mx, my));
                            }
                        }
                    }
                }

                // Update dragged piece position if we're dragging
                if (isDragging && draggedPiece && draggedPiece->sprite) {
                    draggedPiece->sprite->setPosition(sf::Vector2f(mx, my));
                }
            }

            if (event->is<sf::Event::MouseButtonReleased>()) {
                if (!isMyTurn) continue;
                if (event->getIf<sf::Event::MouseButtonReleased>()->button == sf::Mouse::Button::Left) {
                    isMouseDown = false;
                    Position target = screenToBoard(event->getIf<sf::Event::MouseButtonReleased>()->position);

                    if (isDragging && dragConfirmed) {
                        if (std::find(validMoves.begin(), validMoves.end(), target) != validMoves.end()) {
                            if (game.playTurn(dragStartPos, target)) {
                                if (wsClient && ((game.isWhiteHuman() && game.getCurrentTurn() == Color::Black) ||
                                                (game.isBlackHuman() && game.getCurrentTurn() == Color::White))) {
                                    nlohmann::json moveMsg = {
                                        { "type", "move" },
                                        { "from", { { "row", dragStartPos.row }, { "col", dragStartPos.col } } },
                                        { "to",   { { "row", target.row }, { "col", target.col } } }
                                    };
                                    std::cout << "[SEND] GUI sending move: " << moveMsg.dump() << std::endl;
                                    wsClient->sendMessage(moveMsg);
                                }

                                isDragging = false;
                                draggedPiece = nullptr;
                                validMoves.clear();
                                syncSpriteMap();
                                // Handle promotion logic if needed
                                auto movedPiece = game.getBoard().getPieceAt(target);
                                if (movedPiece && movedPiece->getType() == PieceType::Pawn) {
                                    if (movedPiece->getColor() == Color::White && target.row == 7) {
                                        isPromoting = true;
                                        promotionColor = Color::White;
                                        promotionPos = target;
                                        whitePromotionSprite.setPosition(sf::Vector2f(static_cast<float>(target.col * TILE_SIZE), static_cast<float>((7 - target.row) * TILE_SIZE)));
                                    } else if (movedPiece->getColor() == Color::Black && target.row == 0) {
                                        isPromoting = true;
                                        promotionColor = Color::Black;
                                        promotionPos = target;
                                        blackPromotionSprite.setPosition(sf::Vector2f(static_cast<float>(target.col * TILE_SIZE), static_cast<float>((7 - target.row) * TILE_SIZE)));
                                    }
                                }
                                continue;
                            }

                            if (wsClient && dynamic_cast<NetworkPlayer*>(game.getCurrentPlayer())) {
                            nlohmann::json moveMsg = {
                                { "type", "move" },
                                { "from", { { "row", dragStartPos.row }, { "col", dragStartPos.col } } },
                                { "to",   { { "row", target.row }, { "col", target.col } } }
                            };
                                wsClient->sendMessage(moveMsg);
                            }


                        }

                        if (draggedPiece) {
                            if (draggedPiece->sprite) draggedPiece->sprite->setOrigin(sf::Vector2f(0.f, 0.f));
                            if (draggedPiece->sprite) draggedPiece->sprite->setPosition(boardToScreen(dragStartPos));
                        }

                        isDragging = false;
                        draggedPiece = nullptr;
                        validMoves.clear();
                        dragConfirmed = false;
                    } else {
                        // Handle clicking (not dragging)
                        Position clicked = screenToBoard(event->getIf<sf::Event::MouseButtonReleased>()->position);
                        
                        // Check if this was actually a click (mouse didn't move much)
                        int dx = event->getIf<sf::Event::MouseButtonReleased>()->position.x - mouseDownPos.x;
                        int dy = event->getIf<sf::Event::MouseButtonReleased>()->position.y - mouseDownPos.y;
                        bool wasClick = (abs(dx) <= 5 && abs(dy) <= 5);
                        
                        std::cout << "DEBUG: Mouse released at " << clicked.row << "," << clicked.col 
                                  << " dx=" << dx << " dy=" << dy << " wasClick=" << wasClick 
                                  << " hasSelected=" << hasSelected << " selected=" << selected.row << "," << selected.col << std::endl;
                        std::cout << "DEBUG: Raw mouse position: " << event->getIf<sf::Event::MouseButtonReleased>()->position.x 
                                  << "," << event->getIf<sf::Event::MouseButtonReleased>()->position.y << std::endl;
                        
                        if (wasClick) {
                            // Check if we clicked on a piece of our color (to select it)
                            auto clickedPiece = game.getBoard().getPieceAt(clicked);
                            if (clickedPiece && clickedPiece->getColor() == game.getCurrentTurn()) {
                                // Select this piece
                                selected = clicked;
                                hasSelected = true;
                                std::cout << "DEBUG: Selected piece at " << clicked.row << "," << clicked.col << std::endl;
                                
                                // Calculate valid moves for this piece
                                validMoves.clear();
                                for (int r = 0; r < 8; ++r) {
                                    for (int c = 0; c < 8; ++c) {
                                        Position to(r, c);
                                        if (game.isMoveLegal(clicked, to)) {
                                            validMoves.push_back(to);
                                        }
                                    }
                                }
                                std::cout << "DEBUG: Found " << validMoves.size() << " valid moves" << std::endl;
                            } else if (hasSelected) {
                                // We have a piece selected, try to move it (regardless of what we clicked on)
                                std::cout << "DEBUG: Attempting to move from " << selected.row << "," << selected.col 
                                          << " to " << clicked.row << "," << clicked.col << std::endl;
                                
                                // Check if this is a valid move
                                bool isValidMove = false;
                                for (const auto& validMove : validMoves) {
                                    if (validMove == clicked) {
                                        isValidMove = true;
                                        break;
                                    }
                                }
                                
                                if (isValidMove) {
                                    std::cout << "DEBUG: Valid move found, executing..." << std::endl;
                                    Position fromPos = selected; // Store before playTurn changes state
                                    
                                    // Debug: Print the pieces and coordinates
                                    auto fromPiece = game.getBoard().getPieceAt(selected);
                                    auto toPiece = game.getBoard().getPieceAt(clicked);
                                    std::cout << "DEBUG: Calling playTurn from " << selected.row << "," << selected.col 
                                              << " to " << clicked.row << "," << clicked.col << std::endl;
                                    std::cout << "DEBUG: From piece: " << (fromPiece ? "exists" : "nullptr") 
                                              << " To piece: " << (toPiece ? "exists" : "nullptr") << std::endl;
                                    std::cout << "DEBUG: Current turn: " << (game.getCurrentTurn() == Color::White ? "White" : "Black") << std::endl;
                                    
                                    if (game.playTurn(selected, clicked)) {
                                        std::cout << "DEBUG: Move executed successfully!" << std::endl;
                                        if (wsClient) {
                                            nlohmann::json moveMsg = {
                                                { "type", "move" },
                                                { "from", { { "row", fromPos.row }, { "col", fromPos.col } } },
                                                { "to",   { { "row", clicked.row }, { "col", clicked.col } } }
                                            };
                                            std::cout << "[SEND] GUI sending move: " << moveMsg.dump() << std::endl;
                                            wsClient->sendMessage(moveMsg);
                                        }

                                        syncSpriteMap();
                                        auto movedPiece = game.getBoard().getPieceAt(clicked);
                                        if (movedPiece && movedPiece->getType() == PieceType::Pawn) {
                                            if (movedPiece->getColor() == Color::White && clicked.row == 7) {
                                                isPromoting = true;
                                                promotionColor = Color::White;
                                                promotionPos = clicked;
                                                whitePromotionSprite.setPosition(sf::Vector2f(static_cast<float>(clicked.col * TILE_SIZE), static_cast<float>((7 - clicked.row) * TILE_SIZE)));
                                            } else if (movedPiece->getColor() == Color::Black && clicked.row == 0) {
                                                isPromoting = true;
                                                promotionColor = Color::Black;
                                                promotionPos = clicked;
                                                blackPromotionSprite.setPosition(sf::Vector2f(static_cast<float>(clicked.col * TILE_SIZE), static_cast<float>((7 - clicked.row) * TILE_SIZE)));
                                            }
                                        }
                                        hasSelected = false;
                                        validMoves.clear();
                                    } else {
                                        std::cout << "DEBUG: Move execution failed!" << std::endl;
                                        std::cout << "DEBUG: playTurn returned false for move from " << selected.row << "," << selected.col 
                                                  << " to " << clicked.row << "," << clicked.col << std::endl;
                                    }
                                } else {
                                    std::cout << "DEBUG: Invalid move attempt!" << std::endl;
                                    // Clicked on invalid square, clear selection
                                    hasSelected = false;
                                    validMoves.clear();
                                }
                            } else {
                                // No piece selected and clicked on empty/opponent piece, do nothing
                                std::cout << "DEBUG: Clicked on empty/opponent piece with no selection" << std::endl;
                            }
                        }
                    }
                }
            }

            if (event->is<sf::Event::Closed>()) {
                window.close();
                continue;
            }

            if (game.getResult() != GameResult::InProgress)
                continue;

            if (isPromoting) {
                if (const auto* mouseEvent = event->getIf<sf::Event::MouseButtonPressed>()) {
                    if (mouseEvent->button == sf::Mouse::Button::Left) {
                        int px = mouseEvent->position.x - promotionPos.col * TILE_SIZE;
                        int py = mouseEvent->position.y - (7 - promotionPos.row) * TILE_SIZE;

                        PieceType selectedType;
                        if (px < TILE_SIZE / 2 && py < TILE_SIZE / 2)
                            selectedType = PieceType::Rook;
                        else if (px >= TILE_SIZE / 2 && py < TILE_SIZE / 2)
                            selectedType = PieceType::Queen;
                        else if (px < TILE_SIZE / 2 && py >= TILE_SIZE / 2)
                            selectedType = PieceType::Bishop;
                        else
                            selectedType = PieceType::Knight;

                        game.finalizePromotion(promotionPos, selectedType);

                        isPromoting = false;
                        syncSpriteMap();

                        // game.aiAutoMove();
                        syncSpriteMap();
                    }
                }
                continue;
            }

            if (const auto* mouseEvent = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouseEvent->button == sf::Mouse::Button::Left) {
                    // Check if menu button was clicked
                    sf::Vector2f mousePos(static_cast<float>(mouseEvent->position.x), static_cast<float>(mouseEvent->position.y));
                    if (menuBtn.getGlobalBounds().contains(mousePos)) {
                        // Show confirmation dialog
                        sf::RectangleShape overlay(sf::Vector2f(800.f, 800.f));
                        overlay.setFillColor(sf::Color(0, 0, 0, 180));
                        
                        sf::Text confirmText(font, "Return to main menu?", 30);
                        confirmText.setFillColor(sf::Color::White);
                        confirmText.setPosition(sf::Vector2f(300.f, 300.f));
                        
                        sf::RectangleShape yesBtn(sf::Vector2f(100.f, 40.f));
                        yesBtn.setPosition(sf::Vector2f(300.f, 350.f));
                        yesBtn.setFillColor(sf::Color(0, 200, 0));
                        sf::Text yesText(font, "Yes", 20);
                        yesText.setFillColor(sf::Color::Black);
                        yesText.setPosition(sf::Vector2f(320.f, 355.f));
                        
                        sf::RectangleShape noBtn(sf::Vector2f(100.f, 40.f));
                        noBtn.setPosition(sf::Vector2f(420.f, 350.f));
                        noBtn.setFillColor(sf::Color(200, 0, 0));
                        sf::Text noText(font, "No", 20);
                        noText.setFillColor(sf::Color::White);
                        noText.setPosition(sf::Vector2f(450.f, 355.f));
                        
                        bool choosing = true;
                        while (choosing && window.isOpen()) {
                            while (const std::optional<sf::Event> confirmEvent = window.pollEvent()) {
                                if (confirmEvent->is<sf::Event::Closed>()) {
                                    window.close();
                                    return;
                                }
                                if (confirmEvent->is<sf::Event::MouseButtonPressed>() && 
                                    confirmEvent->getIf<sf::Event::MouseButtonPressed>()->button == sf::Mouse::Button::Left) {
                                    sf::Vector2f confirmMousePos(static_cast<float>(confirmEvent->getIf<sf::Event::MouseButtonPressed>()->position.x), 
                                                               static_cast<float>(confirmEvent->getIf<sf::Event::MouseButtonPressed>()->position.y));
                                    if (yesBtn.getGlobalBounds().contains(confirmMousePos)) {
                                        // Return to main menu by throwing an exception that will be caught
                                        throw std::runtime_error("Return to menu");
                                    } else if (noBtn.getGlobalBounds().contains(confirmMousePos)) {
                                        choosing = false;
                                    }
                                }
                            }
                            
                            window.clear();
                            window.draw(boardSprite);
                            
                            // Draw all sprites
                            for (const auto& [pos, sp] : spriteMap) {
                                if (sp.sprite) window.draw(*sp.sprite);
                            }
                            
                            // Draw valid moves
                            for (const auto& pos : validMoves) {
                                auto screenPos = boardToScreen(pos);
                                float x = screenPos.x + TILE_SIZE / 2.f;
                                float y = screenPos.y + TILE_SIZE / 2.f;
                                auto piece = game.getBoard().getPieceAt(pos);
                                if (piece && piece->getColor() != game.getCurrentTurn()) {
                                    possibleCaptureMoveSprite.setPosition(sf::Vector2f(x, y));
                                    window.draw(possibleCaptureMoveSprite);
                                } else {
                                    possibleMoveSprite.setPosition(sf::Vector2f(x, y));
                                    window.draw(possibleMoveSprite);
                                }
                            }
                            
                            window.draw(menuBtn);
                            window.draw(menuLabel);
                            
                            // Draw overlay
                            window.draw(overlay);
                            window.draw(confirmText);
                            window.draw(yesBtn);
                            window.draw(yesText);
                            window.draw(noBtn);
                            window.draw(noText);
                            window.display();
                        }
                        continue;
                    }
                    
                    if (!isMyTurn) continue;
                    isMouseDown = true;
                    mouseDownPos = mouseEvent->position;
                    
                    // Check if we clicked on a valid piece
                    Position clicked = screenToBoard(mouseEvent->position);
                    auto piece = game.getBoard().getPieceAt(clicked);
                    if (piece && piece->getColor() == game.getCurrentTurn()) {
                        // Store the clicked piece for potential drag or click
                        dragStartPos = clicked;
                        selected = clicked;
                        hasSelected = true;
                        
                        // Calculate valid moves for this piece immediately
                        validMoves.clear();
                        for (int r = 0; r < 8; ++r) {
                            for (int c = 0; c < 8; ++c) {
                                Position to(r, c);
                                if (game.isMoveLegal(clicked, to)) {
                                    validMoves.push_back(to);
                                }
                            }
                        }
                        std::cout << "DEBUG: Mouse pressed on piece at " << clicked.row << "," << clicked.col 
                                  << " found " << validMoves.size() << " valid moves" << std::endl;
                    } else if (!hasSelected) {
                        // Only clear selection if we don't already have a piece selected
                        // Clicked on empty square or opponent piece, clear selection
                        hasSelected = false;
                        validMoves.clear();
                        std::cout << "DEBUG: Mouse pressed on empty/opponent piece at " << clicked.row << "," << clicked.col << std::endl;
                    } else {
                        // We have a piece selected and clicked on empty/opponent piece
                        // Don't clear selection - let the mouse button released event handle the move
                        std::cout << "DEBUG: Mouse pressed on empty/opponent piece with selection, keeping selection" << std::endl;
                    }
                }
            }
        }

        if (waitingForMatch) {
            // Only show waiting message, do not process moves or allow interaction
            sf::Font statusFont;
            (void)statusFont.openFromFile("assets/fonts/Arial.ttf");
            sf::Text statusText(statusFont, statusMessage, 40);
            statusText.setFillColor(sf::Color::White);
            statusText.setPosition(sf::Vector2f(300.f, 350.f));
            window.clear();
            window.draw(boardSprite);
            window.draw(statusText);
            window.display();
            continue;
        }

        window.clear();
        window.draw(boardSprite);

        for (const auto& pos : validMoves) {
            auto screenPos = boardToScreen(pos);
            float x = screenPos.x + TILE_SIZE / 2.f;
            float y = screenPos.y + TILE_SIZE / 2.f;
            auto piece = game.getBoard().getPieceAt(pos);
            if (piece && piece->getColor() != game.getCurrentTurn()) {
                possibleCaptureMoveSprite.setPosition(sf::Vector2f(x, y));
                window.draw(possibleCaptureMoveSprite);
            } else {
                possibleMoveSprite.setPosition(sf::Vector2f(x, y));
                window.draw(possibleMoveSprite);
            }
        }

        for (const auto& [pos, sp] : spriteMap) {
            if (isDragging && pos == dragStartPos) continue;
            if (sp.sprite) window.draw(*sp.sprite);
        }

        // draw dragged piece last so it's on top
        if (isDragging && draggedPiece && draggedPiece->sprite) {
            window.draw(*draggedPiece->sprite);
        }

        if (isPromoting) {
            spriteMap.erase(promotionPos);
            if (promotionColor == Color::White)
                window.draw(whitePromotionSprite);
            else
                window.draw(blackPromotionSprite);
        }

        window.draw(menuBtn);
        window.draw(menuLabel);

        window.display();
    }
}

AppState runMainMenu(sf::RenderWindow& window, const Config& cfg) {
    sf::Font font;
    if (!font.openFromFile("assets/fonts/Arial.ttf")) {
        std::cerr << "Failed to load font for main menu.\n";
        return AppState::Quit;
    }

    // Gradient background: black to green
    sf::VertexArray gradient(sf::PrimitiveType::TriangleStrip, 4);
    gradient[0].position = sf::Vector2f(0.f, 0.f);
    gradient[1].position = sf::Vector2f(800.f, 0.f);
    gradient[2].position = sf::Vector2f(0.f, 800.f);
    gradient[3].position = sf::Vector2f(800.f, 800.f);
    gradient[0].color = sf::Color::Black;
    gradient[1].color = sf::Color(0, 60, 0);
    gradient[2].color = sf::Color(0, 120, 0);
    gradient[3].color = sf::Color(0, 200, 0);

    sf::Text title(font, "Chessmate", 70);
    title.setFillColor(sf::Color::White);
    title.setStyle(sf::Text::Bold);
    title.setPosition(sf::Vector2f(300.f, 80.f));

    std::vector<Button> buttons;
    std::vector<std::string> labels = {"Local", "Online", "Play vs AI", "Quit"};
    float btnWidth = 350, btnHeight = 70, spacing = 30;
    float startY = 250;
    for (int i = 0; i < 4; ++i) {
        Button btn{sf::RectangleShape(), sf::Text(font, labels[static_cast<size_t>(i)], 36), nullptr, false};
        btn.rect.setSize(sf::Vector2f(btnWidth, btnHeight));
        btn.rect.setOrigin({btnWidth / 2, btnHeight / 2});
        btn.rect.setPosition({400.f, startY + static_cast<float>(i) * (btnHeight + spacing)});
        btn.rect.setFillColor(sf::Color(30, 30, 30, 220));
        btn.rect.setOutlineThickness(3);
        btn.rect.setOutlineColor(sf::Color(0, 200, 0));
        btn.label.setFillColor(sf::Color(200, 255, 200));
        btn.label.setStyle(sf::Text::Bold);
        btn.label.setPosition({350.f, btn.rect.getPosition().y - 15.f});
        buttons.push_back(btn);
    }

    // Button actions
    buttons[0].onClick = [&]() { 
        window.clear(); 
        window.display(); 
        try {
            runGUI(window, nullptr, false, true);
        } catch (const std::runtime_error& e) {
            if (std::string(e.what()) == "Return to menu") {
                // Exception will be caught by main.cpp
                throw;
            }
        }
    };
    buttons[1].onClick = [&]() { 
        window.clear(); 
        window.display(); 
        cm::WebSocketClient* wsClient = new cm::WebSocketClient(cfg.network.ws_url, cfg.network.reconnect_ms); 
        wsClient->connect(cfg.network.ws_url); 
        try {
            runGUI(window, wsClient, false, true);
        } catch (const std::runtime_error& e) {
            if (std::string(e.what()) == "Return to menu") {
                // Exception will be caught by main.cpp
                throw;
            }
        }
        delete wsClient; 
    };
    buttons[2].onClick = [&]() {
        // Color selection overlay
        sf::RectangleShape overlay(sf::Vector2f(800.f, 800.f));
        overlay.setFillColor(sf::Color(0, 0, 0, 180));
        sf::Text prompt(font, "Choose your color", 40);
        prompt.setFillColor(sf::Color(200, 255, 200));
        prompt.setPosition({400.f - prompt.getLocalBounds().position.x / 2.f, 200.f});
        sf::RectangleShape whiteBtn(sf::Vector2f(200.f, 70.f));
        whiteBtn.setOrigin(sf::Vector2f(100.f, 35.f));
        whiteBtn.setPosition(sf::Vector2f(250.f, 400.f));
        whiteBtn.setFillColor(sf::Color(220, 220, 220));
        sf::Text whiteLabel(font, "White", 32);
        whiteLabel.setFillColor(sf::Color::Black);
        whiteLabel.setPosition({250.f - whiteLabel.getLocalBounds().position.x / 2.f, 400.f - whiteLabel.getLocalBounds().position.y / 1.5f});
        sf::RectangleShape blackBtn(sf::Vector2f(200.f, 70.f));
        blackBtn.setOrigin(sf::Vector2f(100.f, 35.f));
        blackBtn.setPosition(sf::Vector2f(550.f, 400.f));
        blackBtn.setFillColor(sf::Color(30, 30, 30));
        sf::Text blackLabel(font, "Black", 32);
        blackLabel.setFillColor(sf::Color(200, 255, 200));
        blackLabel.setPosition({550.f - blackLabel.getLocalBounds().position.x / 2.f, 400.f - blackLabel.getLocalBounds().position.y / 1.5f});
        bool choosing = true;
        while (choosing && window.isOpen()) {
            // sf::Event e;
            while (const std::optional event = window.pollEvent()) {
                if (event->is<sf::Event::Closed>()) { window.close(); return; }
                if (event->is<sf::Event::MouseButtonPressed>() && event->getIf<sf::Event::MouseButtonPressed>()->button == sf::Mouse::Button::Left) {
                    sf::Vector2f moustPos(static_cast<float>(event->getIf<sf::Event::MouseButtonPressed>()->position.x), static_cast<float>(event->getIf<sf::Event::MouseButtonPressed>()->position.y));
                    if (whiteBtn.getGlobalBounds().contains(moustPos)) {
                        window.clear(); window.display(); 
                        try {
                            runGUI(window, nullptr, true, true);
                        } catch (const std::runtime_error& e) {
                            if (std::string(e.what()) == "Return to menu") {
                                throw;
                            }
                        }
                        return;
                    }
                    if (blackBtn.getGlobalBounds().contains(moustPos)) {
                        window.clear(); window.display(); 
                        try {
                            runGUI(window, nullptr, true, false);
                        } catch (const std::runtime_error& e) {
                            if (std::string(e.what()) == "Return to menu") {
                                throw;
                            }
                        }
                        return;
                    }
                }
            }
            window.clear();
            window.draw(overlay);
            window.draw(prompt);
            window.draw(whiteBtn); window.draw(whiteLabel);
            window.draw(blackBtn); window.draw(blackLabel);
            window.display();
        }
    };
    buttons[3].onClick = [&]() { window.close(); };

    while (window.isOpen()) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        for (auto& btn : buttons) {
            btn.hovered = btn.rect.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos));
            btn.rect.setFillColor(btn.hovered ? sf::Color(0, 200, 0, 220) : sf::Color(30, 30, 30, 220));
            btn.label.setFillColor(btn.hovered ? sf::Color::White : sf::Color(200, 255, 200));
        }
        // sf::Event event;
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
                return AppState::Quit;
            }
            if (event->is<sf::Event::MouseButtonPressed>() && event->getIf<sf::Event::MouseButtonPressed>()->button == sf::Mouse::Button::Left) {
                for (auto& btn : buttons) {
                    if (btn.hovered && btn.onClick) {
                        btn.onClick();
                        return AppState::Game;
                    }
                }
            }
        }
        window.clear();
        window.draw(gradient);
        window.draw(title);
        for (const auto& btn : buttons) {
            window.draw(btn.rect);
            window.draw(btn.label);
        }
        window.display();
    }
    return AppState::Quit;
}

AppState runGame(sf::RenderWindow& window, const Config& /*cfg*/) {
    // For now, just run the GUI and return to menu
    runGUI(window, nullptr, false, true);
    return AppState::Menu;
}

}

