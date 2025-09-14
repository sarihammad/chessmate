#pragma once

#include "domain/player.hpp"
#include "infrastructure/network/websocket_client.hpp"

namespace chess {

class NetworkPlayer : public Player {
public:
    NetworkPlayer(Color playerColor, cm::WebSocketClient& wsClient);
    bool makeMove(Board& board, Position& from, Position& to) override;

private:
    cm::WebSocketClient& client; // For future network message handling
};

}