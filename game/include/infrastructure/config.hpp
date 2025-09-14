#pragma once
#include <string>
#include <optional>

namespace chess {
struct NetworkCfg { 
  std::string ws_url{"ws://localhost:8080/game"}; 
  int reconnect_ms{2000}; 
};
struct UiCfg { 
  int window_width{800}; 
  int window_height{800}; 
  bool vsync{true}; 
};
struct Config { 
  NetworkCfg network{}; 
  UiCfg ui{}; 
};

Config load_config(std::optional<std::string> path = std::nullopt);
}