#pragma once

#if !defined(ASIO_STANDALONE)
#define ASIO_STANDALONE
#endif

#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <functional>
#include <thread>
#include <atomic>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <nlohmann/json.hpp>
#include "utils/utils.hpp"

using websocketpp::connection_hdl;
using json = nlohmann::json;
using MessageHandler = std::function<void(const json&)>;

namespace chess {

class WebSocketClient {
public:
    
    WebSocketClient();
    ~WebSocketClient();

    void connect(const std::string& uri);
    void sendMessage(const json& message);
    void setMessageHandler(MessageHandler handler);
    void close();
    void setMoveHandler(std::function<void(Position, Position)> handler);
    bool isConnected() const { return connected; }

    std::function<void(Position, Position)> onMoveReceived;

private:
    void onOpen(connection_hdl hdl);
    void onMessage(connection_hdl hdl, websocketpp::config::asio_client::message_type::ptr msg);
    void onClose(connection_hdl hdl);
    void run();

    websocketpp::client<websocketpp::config::asio_client> client;
    connection_hdl connection;
    std::thread clientThread;
    std::atomic<bool> connected;
    MessageHandler messageHandler;
};

}