#include "infrastructure/network/websocket_client.hpp"
#include <chrono>
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>

using namespace std::chrono_literals;

namespace cm {
WebSocketClient::WebSocketClient(std::string url, int reconnect_ms)
  : url_(std::move(url)), reconnect_ms_(reconnect_ms) {}

WebSocketClient::~WebSocketClient() { stop(); }

void WebSocketClient::on_message(MessageHandler cb) { on_msg_ = std::move(cb); }
void WebSocketClient::on_state(StateHandler cb) { on_state_ = std::move(cb); }

bool WebSocketClient::send(const std::string& text) {
  std::scoped_lock lk(q_mtx_);
  if (outbox_.size() >= max_queue_) return false;
  outbox_.push(text);
  return true;
}

void WebSocketClient::start() {
  if (th_.joinable()) return;
  th_ = std::thread([this]() { run_(); });
}

void WebSocketClient::stop() {
  should_stop_.store(true);
  if (th_.joinable()) th_.join();
}

void WebSocketClient::run_() {
  while (!should_stop_.load()) {
    try {
      // TODO: connect using your websocketpp/asio client to url_
      connected_.store(true);
      if (on_state_) on_state_(true);
      spdlog::info("WebSocket connected: {}", url_);

      // Pump loop (pseudo): send queued, read messages, ping/pong, etc.
      while (!should_stop_.load()) {
        // send queue
        {
          std::scoped_lock lk(q_mtx_);
          while (!outbox_.empty()) {
            // const auto& msg = outbox_.front();
            // ws.send(msg);
            outbox_.pop();
          }
        }
        // read one message (blocking with timeout) -> if received:
        // if (on_msg_) on_msg_(payload);

        std::this_thread::sleep_for(10ms);
      }
      // ws.close()
      connected_.store(false);
      if (on_state_) on_state_(false);
      spdlog::info("WebSocket stopping cleanly.");
      return;
    } catch (const std::exception& e) {
      connected_.store(false);
      if (on_state_) on_state_(false);
      spdlog::warn("WebSocket error: {}. Reconnecting in {} ms", e.what(), reconnect_ms_);
      std::this_thread::sleep_for(std::chrono::milliseconds(reconnect_ms_));
    }
  }
}

// Legacy compatibility methods
void WebSocketClient::connect(const std::string& /*uri*/) {
  // Update URL and start if not already running
  if (!th_.joinable()) {
    start();
  }
}

void WebSocketClient::sendMessage(const nlohmann::json& message) {
  send(message.dump());
}

void WebSocketClient::setMessageHandler(MessageHandler handler) {
  on_message(std::move(handler));
}

void WebSocketClient::close() {
  stop();
}
} // namespace cm