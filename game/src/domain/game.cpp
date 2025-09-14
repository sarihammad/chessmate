#include "domain/pieces/movable_piece.hpp"
#include "domain/pieces/pawn.hpp"
#include "domain/pieces/rook.hpp"
#include "domain/pieces/knight.hpp"
#include "domain/pieces/bishop.hpp"
#include "domain/pieces/queen.hpp"
#include "domain/game.hpp"
#include "infrastructure/players/network_player.hpp"
#include <SFML/Audio.hpp>
#include <iostream>
#include <cstdlib>


namespace chess {

Game::Game() : currentTurn(Color::White), result(GameResult::InProgress) {
    std::cout << "Game constructor called" << std::endl;
    board.setupInitialPosition();
    std::cout << "Board setup complete" << std::endl;
    try {
        loadSounds();
        std::cout << "Sounds loaded successfully" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Failed to load sounds: " << e.what() << std::endl;
    }
}

void Game::loadSounds() {
    if (!soundBuffers["checkmate_win"].loadFromFile("assets/sounds/checkmate_win.wav")) {
        throw std::runtime_error("Failed to load checkmate_win.wav");
    }
    if (!soundBuffers["checkmate_loss"].loadFromFile("assets/sounds/checkmate_loss.wav")) {
        throw std::runtime_error("Failed to load checkmate_loss.wav");
    }
    if (!soundBuffers["game_start"].loadFromFile("assets/sounds/game_start.wav")) {
        throw std::runtime_error("Failed to load game_start.wav");
    }
    if (!soundBuffers["move"].loadFromFile("assets/sounds/move.wav")) {
        throw std::runtime_error("Failed to load move.wav sound file");
    }
    if (!soundBuffers["capture"].loadFromFile("assets/sounds/capture.wav")) {
        throw std::runtime_error("Failed to load capture.wav sound file");
    }
    if (!soundBuffers["castle"].loadFromFile("assets/sounds/castle.wav")) {
        throw std::runtime_error("Failed to load castle.wav sound file");
    }
    if (!soundBuffers["check"].loadFromFile("assets/sounds/check.wav")) {
        throw std::runtime_error("Failed to load check.wav sound file");
    }
    if (!soundBuffers["stalemate"].loadFromFile("assets/sounds/stalemate.wav")) {
        throw std::runtime_error("Failed to load stalemate.wav sound file");
    }
    if (!soundBuffers["promote"].loadFromFile("assets/sounds/promote.wav")) {
        throw std::runtime_error("Failed to load promote.wav sound file");
    }

    sounds.emplace("game_start", sf::Sound(soundBuffers["game_start"]));
    sounds.emplace("move", sf::Sound(soundBuffers["move"]));
    sounds.emplace("capture", sf::Sound(soundBuffers["capture"]));
    sounds.emplace("castle", sf::Sound(soundBuffers["castle"]));
    sounds.emplace("check", sf::Sound(soundBuffers["check"]));
    sounds.emplace("stalemate", sf::Sound(soundBuffers["stalemate"]));
    sounds.emplace("promote", sf::Sound(soundBuffers["promote"]));
    sounds.emplace("checkmate_win", sf::Sound(soundBuffers["checkmate_win"]));
    sounds.emplace("checkmate_loss", sf::Sound(soundBuffers["checkmate_loss"]));
}

bool Game::playTurn(Position from, Position to) {
    if (result != GameResult::InProgress) return false;
    if (!whitePlayer || !blackPlayer) throw std::runtime_error("Players not set in Game");
    auto& currentPlayer = (currentTurn == Color::White) ? whitePlayer : blackPlayer;
    
    auto targetBeforeMove = board.getPieceAt(to);
    bool isDiagonalEnPassant =
        board.getPieceAt(from) && board.getPieceAt(from)->getType() == PieceType::Pawn &&
        board.isEmpty(to) &&
        abs(from.col - to.col) == 1 &&
        abs(from.row - to.row) == 1;
    bool isCapture = (targetBeforeMove && targetBeforeMove->getColor() != currentTurn) || isDiagonalEnPassant;

    // For network games, the server is the source of truth, so we don't re-validate the move.
    // For local games, the HumanPlayer's makeMove already validates.
    if (dynamic_cast<NetworkPlayer*>(currentPlayer.get())) {
        board.movePiece(from, to);
    } else {
        bool success = currentPlayer->makeMove(board, from, to);
        if (!success) return false;
    }

    auto movedPiece = board.getPieceAt(to);
    auto movable = std::dynamic_pointer_cast<MovablePiece>(movedPiece);
    if (movable) {
        movable->markMoved();
    }

    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            auto p = board.getPieceAt(Position(r, c));
            if (p && p->getType() == PieceType::Pawn) {
                auto pawn = std::dynamic_pointer_cast<Pawn>(p);
                if (pawn) pawn->setEnPassantCapturable(false);
            }
        }
    }

    if (movedPiece && movedPiece->getType() == PieceType::Pawn && abs(to.row - from.row) == 2) {
        auto pawn = std::dynamic_pointer_cast<Pawn>(movedPiece);
        if (pawn) pawn->setEnPassantCapturable(true);
    }

    if (movedPiece && movedPiece->getType() == PieceType::King && abs(to.col - from.col) == 2) {
        int rookFromCol = (to.col > from.col) ? 7 : 0;
        int rookToCol   = (to.col > from.col) ? to.col - 1 : to.col + 1;

        Position rookFrom(to.row, rookFromCol);
        Position rookTo(to.row, rookToCol);

        board.movePiece(rookFrom, rookTo);

        auto rook = board.getPieceAt(rookTo);
        auto rookMovable = std::dynamic_pointer_cast<MovablePiece>(rook);
        if (rookMovable) rookMovable->markMoved();
    }

    auto movedPieceAfter = board.getPieceAt(to);
    bool isCastling = movedPieceAfter && movedPieceAfter->getType() == PieceType::King && abs(to.col - from.col) == 2;

    Color opponent = getOtherColor(currentTurn);
    bool check = board.is_check(opponent);
    bool moves = hasValidMove(opponent);

    std::string soundToPlay;

    if (!moves && check) {
        result = (currentTurn == Color::White) ? GameResult::WhiteWins : GameResult::BlackWins;
        soundToPlay = (result == GameResult::WhiteWins) ? "checkmate_win" : "checkmate_loss";
    } else if (!moves && !check) {
        result = GameResult::Stalemate;
        soundToPlay = "stalemate";
    } else {
        if (isInsufficientMaterial()) {
            result = GameResult::Stalemate;
            soundToPlay = "stalemate";
        } else {
            currentTurn = opponent;
            if (check) soundToPlay = "check";
            else if (isCastling) soundToPlay = "castle";
            else if (isCapture) soundToPlay = "capture";
            else soundToPlay = "move";
        }
    }

    playSound(soundToPlay);

    return true;
}

void Game::aiAutoMove() {
    if (result != GameResult::InProgress || currentTurn != Color::Black) return;
    if (!blackPlayer) throw std::runtime_error("Black player not set in Game");

    Position from(-1, -1), to(-1, -1);
    blackPlayer->makeMove(board, from, to);

    Color opponent = getOtherColor(currentTurn);
    bool check = board.is_check(opponent);
    bool moves = hasValidMove(opponent);

    if (!moves && check) {
        result = GameResult::BlackWins;
    } else if (!moves && !check) {
        result = GameResult::Stalemate;
    } else {
        currentTurn = opponent;
    }
}

Board& Game::getBoard() {
    return board;
}

const Board& Game::getBoard() const {
    return board;
}

Color Game::getCurrentTurn() const {
    return currentTurn;
}

GameResult Game::getResult() const {
    return result;
}

bool Game::isGameOver() const {
    return result != GameResult::InProgress;
}

bool Game::hasValidMove(Color color) {
    for (int r1 = 0; r1 < 8; ++r1) {
        for (int c1 = 0; c1 < 8; ++c1) {
            Position from(r1, c1);
            auto piece = board.getPieceAt(from);
            if (!piece || piece->getColor() != color) continue;

            for (int r2 = 0; r2 < 8; ++r2) {
                for (int c2 = 0; c2 < 8; ++c2) {
                    Position to(r2, c2);
                    if (piece->isValidMove(from, to, board)) {
                        Board copy = board.clone();
                        copy.movePiece(from, to);
                        bool stillInCheck = isCheckOnBoard(copy, color);
                        if (!stillInCheck) return true;
                    }
                }
            }
        }
    }
    return false;
}

bool Game::isWhiteHuman() const {
    return dynamic_cast<HumanPlayer*>(whitePlayer.get()) != nullptr;
}

bool Game::isBlackHuman() const {
    return dynamic_cast<HumanPlayer*>(blackPlayer.get()) != nullptr;
}

bool Game::isCheckOnBoard(const Board& b, Color color) const {
    Position kingPos(-1, -1);
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            Position pos(r, c);
            auto piece = b.getPieceAt(pos);
            if (piece && piece->getType() == PieceType::King && piece->getColor() == color)
                kingPos = pos;
        }
    }

    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            Position attackerPos(r, c);
            auto attacker = b.getPieceAt(attackerPos);
            if (attacker && attacker->getColor() != color) {
                if (attacker->isValidMove(attackerPos, kingPos, b))
                    return true;
            }
        }
    }

    return false;
}

bool Game::isMoveLegal(Position from, Position to) {
    Board copy = board.clone();
    auto piece = copy.getPieceAt(from);
    if (!piece || !piece->isValidMove(from, to, copy)) return false;

    copy.movePiece(from, to);
    return !isCheckOnBoard(copy, piece->getColor());
}

bool Game::isInsufficientMaterial() const {
    std::vector<std::shared_ptr<Piece>> whitePieces;
    std::vector<std::shared_ptr<Piece>> blackPieces;
    std::vector<Position> whiteBishopSquares;
    std::vector<Position> blackBishopSquares;

    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            Position pos(r, c);
            auto piece = board.getPieceAt(pos);
            if (!piece) continue;

            if (piece->getColor() == Color::White) {
                whitePieces.push_back(piece);
                if (piece->getType() == PieceType::Bishop) whiteBishopSquares.push_back(pos);
            } else {
                blackPieces.push_back(piece);
                if (piece->getType() == PieceType::Bishop) blackBishopSquares.push_back(pos);
            }
        }
    }

    auto isOnlyKing = [](const std::vector<std::shared_ptr<Piece>>& pieces) {
        return pieces.size() == 1 && pieces[0]->getType() == PieceType::King;
    };

    auto isKingAndMinor = [](const std::vector<std::shared_ptr<Piece>>& pieces) {
        if (pieces.size() != 2) return false;
        int countKing = 0, countMinor = 0;
        for (const auto& p : pieces) {
            if (p->getType() == PieceType::King) countKing++;
            if (p->getType() == PieceType::Bishop || p->getType() == PieceType::Knight) countMinor++;
        }
        return countKing == 1 && countMinor == 1;
    };

    // king vs king
    if (isOnlyKing(whitePieces) && isOnlyKing(blackPieces)) return true;

    // king and bishop/knight vs king
    if ((isKingAndMinor(whitePieces) && isOnlyKing(blackPieces)) ||
        (isOnlyKing(whitePieces) && isKingAndMinor(blackPieces))) return true;

    // king and bishop vs king and bishop on same color squares
    if (whitePieces.size() == 2 && blackPieces.size() == 2 &&
        whiteBishopSquares.size() == 1 && blackBishopSquares.size() == 1) {
        auto whiteSq = whiteBishopSquares[0];
        auto blackSq = blackBishopSquares[0];

        bool whiteIsLight = (whiteSq.row + whiteSq.col) % 2 == 0;
        bool blackIsLight = (blackSq.row + blackSq.col) % 2 == 0;

        if (whiteIsLight == blackIsLight) return true;
    }

    return false;
}

void Game::playSound(const std::string& name) {
    auto it = sounds.find(name);
    if (it != sounds.end()) {
        it->second.play();
    }
}

void Game::finalizePromotion(Position pos, PieceType newType) {
    Color color = board.getPieceAt(pos)->getColor();

    // replace pawn with the new piece
    std::shared_ptr<Piece> newPiece;
    switch (newType) {
        case PieceType::Rook: newPiece = std::make_shared<Rook>(color); break;
        case PieceType::Knight: newPiece = std::make_shared<Knight>(color); break;
        case PieceType::Bishop: newPiece = std::make_shared<Bishop>(color); break;
        case PieceType::Queen:
        default: newPiece = std::make_shared<Queen>(color); break;
    }

    board.setPieceAt(pos, newPiece);

    // playSound("promote");

    //determine game state after promotion
    Color opponent = getOtherColor(color);
    bool check = board.is_check(opponent);
    bool moves = hasValidMove(opponent);

    std::string soundToPlay;

    if (!moves && check) {
        result = (color == Color::White) ? GameResult::WhiteWins : GameResult::BlackWins;
        soundToPlay = (result == GameResult::WhiteWins) ? "checkmate_win" : "checkmate_loss";
    } else if (!moves && !check) {
        result = GameResult::Stalemate;
        soundToPlay = "stalemate";
    } else if (check) {
        soundToPlay = "check";
    } else {
        soundToPlay = "promote";
    }

    playSound(soundToPlay);
}

void Game::setWhitePlayer(std::unique_ptr<Player> player) {
    whitePlayer = std::move(player);
}
void Game::setBlackPlayer(std::unique_ptr<Player> player) {
    blackPlayer = std::move(player);
}
Player* Game::getCurrentPlayer() {
    return (currentTurn == Color::White) ? whitePlayer.get() : blackPlayer.get();
}

}
