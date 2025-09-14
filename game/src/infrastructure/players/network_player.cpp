#include "infrastructure/players/network_player.hpp"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace chess {

NetworkPlayer::NetworkPlayer(Color playerColor, cm::WebSocketClient& wsClient)
    : Player(playerColor), client(wsClient) {
    // The "join" message is now sent from the GUI, not here.
    // Store reference to client for future network message handling
    (void)client; // Suppress unused variable warning
}

bool NetworkPlayer::makeMove(Board& board, Position& from, Position& to) {
    if (!from.isValid() || !to.isValid()) return false;

    auto piece = board.getPieceAt(from);
    if (!piece || piece->getColor() != color) return false;
    if (!piece->isValidMove(from, to, board)) return false;

    // A network player reflects a move received from the server.
    // We just update the board; we DO NOT send a message back.
    board.movePiece(from, to);

    return true;
}

}