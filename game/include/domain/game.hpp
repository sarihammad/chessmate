#pragma once

#include "board.hpp"
#include "domain/player.hpp"
#include "infrastructure/players/human_player.hpp"
#include "infrastructure/players/ai_player.hpp"
#include "utils/utils.hpp"
#include "domain/enums.hpp"
#include <memory>
#include <unordered_map>
#include <SFML/Audio.hpp>

namespace chess {

enum class GameResult {
    InProgress,
    WhiteWins,
    BlackWins,
    Stalemate
};

class Game {
private:
    Board board;
    std::unique_ptr<Player> whitePlayer;
    std::unique_ptr<Player> blackPlayer;
    Color currentTurn;
    GameResult result;
    std::unordered_map<std::string, sf::SoundBuffer> soundBuffers;
    std::unordered_map<std::string, sf::Sound> sounds;
    void loadSounds();
    // Game owns the board, the players, the game state, and all game logic.

public:
    Game();

    bool playTurn(Position from, Position to);
    void aiAutoMove();

    Board& getBoard();
    const Board& getBoard() const;
    Color getCurrentTurn() const;
    GameResult getResult() const;

    bool isCheck(Color color) const;
    bool isCheckOnBoard(const Board& b, Color color) const;

    bool isWhiteHuman() const;
    bool isBlackHuman() const;

    void setWhitePlayer(std::unique_ptr<Player> player);
    void setBlackPlayer(std::unique_ptr<Player> player);
    Player* getCurrentPlayer();

    bool isMoveLegal(Position from, Position to);
    bool isInsufficientMaterial() const;

    void playSound(const std::string& name);
    void finalizePromotion(Position pos, PieceType newType);

    bool isGameOver() const;

private:
    bool hasValidMove(Color color);
};

}