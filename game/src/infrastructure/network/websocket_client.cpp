#include "infrastructure/network/websocket_client.hpp"
#include <iostream>
#include "utils/utils.hpp"

namespace chess {

WebSocketClient::WebSocketClient() : connected(false) {
    client.init_asio();
    client.set_open_handler([this](connection_hdl hdl) { this->onOpen(hdl); });
    client.set_message_handler([this](connection_hdl hdl, auto msg) { this->onMessage(hdl, msg); });
    client.set_close_handler([this](connection_hdl hdl) { this->onClose(hdl); });
}

WebSocketClient::~WebSocketClient() {
    close();
    if (clientThread.joinable()) clientThread.join();
}

void WebSocketClient::connect(const std::string& uri) {
    websocketpp::lib::error_code ec;
    auto con = client.get_connection(uri, ec);
    if (ec) {
        std::cerr << "Connection error: " << ec.message() << std::endl;
        return;
    }
    connection = con->get_handle();
    client.connect(con);
    clientThread = std::thread([this]() { this->run(); });
}

void WebSocketClient::run() {
    client.run();
}

void WebSocketClient::sendMessage(const json& message) {
    if (connected) {
        std::string payload = message.dump();
        client.send(connection, payload, websocketpp::frame::opcode::text);
    }
}


void WebSocketClient::onOpen(connection_hdl hdl) {
    connected = true;
    std::cout << "WebSocket connected!" << std::endl;
}

void WebSocketClient::onMessage(websocketpp::connection_hdl, websocketpp::config::asio_client::message_type::ptr msg) {
    try {
        auto payload = json::parse(msg->get_payload());

        // Always deliver to messageHandler
        if (messageHandler) {
            messageHandler(payload);
        }

        // Only call onMoveReceived for opponentMove
        if (payload["type"] == "opponentMove") {
            int fromRow = payload["from"]["row"];
            int fromCol = payload["from"]["col"];
            int toRow   = payload["to"]["row"];
            int toCol   = payload["to"]["col"];
            if (onMoveReceived) {
                onMoveReceived(Position(fromRow, fromCol), Position(toRow, toCol));
            }
        }
    } catch (...) {
        std::cerr << "Invalid JSON received.\n";
    }
}

void WebSocketClient::onClose(connection_hdl) {
    connected = false;
    std::cout << "WebSocket closed." << std::endl;
}

void WebSocketClient::close() {
    if (connected) {
        websocketpp::lib::error_code ec;
        client.close(connection, websocketpp::close::status::normal, "Shutdown", ec);
    }
}

void WebSocketClient::setMoveHandler(std::function<void(Position, Position)> handler) {
    onMoveReceived = std::move(handler);
}

void WebSocketClient::setMessageHandler(MessageHandler handler) {
    messageHandler = std::move(handler);
}

}