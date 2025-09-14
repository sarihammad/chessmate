#pragma once
#include <atomic>
#include <functional>
#include <optional>
#include <queue>
#include <string>
#include <mutex>
#include <thread>
#include <nlohmann/json.hpp>

namespace cm {
class WebSocketClient {
public:
  using MessageHandler = std::function<void(const std::string&)>;
  using StateHandler   = std::function<void(bool /*connected*/)> ;

  explicit WebSocketClient(std::string url, int reconnect_ms);
  ~WebSocketClient();

  void start();
  void stop();
  bool is_connected() const noexcept { return connected_.load(); }

  void on_message(MessageHandler cb);
  void on_state(StateHandler cb);
  bool send(const std::string& text); // enqueue; bounded

  // Legacy compatibility methods
  void connect(const std::string& uri);
  void sendMessage(const nlohmann::json& message);
  void setMessageHandler(MessageHandler handler);
  void close();

private:
  void run_();

  const std::string url_;
  const int reconnect_ms_;
  std::thread th_;
  std::atomic<bool> connected_{false};
  std::atomic<bool> should_stop_{false};
  MessageHandler on_msg_;
  StateHandler   on_state_;
  std::mutex q_mtx_;
  std::queue<std::string> outbox_;
  const size_t max_queue_ = 1024;
};
} // namespace cm