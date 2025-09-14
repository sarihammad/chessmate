#pragma once
#include <string>
#include <optional>

namespace cm {
struct NetworkCfg {
  std::string ws_url{"ws://localhost:8080/game"};
  int reconnect_ms{2000};
};
struct UiCfg {
  int  window_width{900};
  int  window_height{900};
  bool vsync{true};
};
struct Config {
  NetworkCfg network{};
  UiCfg      ui{};
};

// Loads config from JSON file path (optional), applies env overrides, returns defaults on failure.
Config load_config(std::optional<std::string> path = std::nullopt);
} // namespace cm