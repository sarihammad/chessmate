#include "infrastructure/config.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <cstdlib>

using nlohmann::json;
namespace chess {
static std::string get_env(const char* k, const std::string& d){ if(const char* v=getenv(k)) return v; return d; }
static int get_env_i(const char* k, int d){ if(const char* v=getenv(k)) return std::atoi(v); return d; }

Config load_config(std::optional<std::string> path) {
  Config cfg{};
  if (path && !path->empty()) {
    try {
      std::ifstream in(*path);
      if (in) {
        json j; in >> j;
        if (j.contains("network")) {
          const auto& n=j["network"];
          if (n.contains("ws_url")) cfg.network.ws_url=n["ws_url"].get<std::string>();
          if (n.contains("reconnect_ms")) cfg.network.reconnect_ms=n["reconnect_ms"].get<int>();
        }
        if (j.contains("ui")) {
          const auto& u=j["ui"];
          if (u.contains("window_width"))  cfg.ui.window_width = u["window_width"].get<int>();
          if (u.contains("window_height")) cfg.ui.window_height= u["window_height"].get<int>();
          if (u.contains("vsync"))         cfg.ui.vsync        = u["vsync"].get<bool>();
        }
      }
    } catch (...) { /* keep defaults */ }
  }
  cfg.network.ws_url      = get_env("CHESSMATE_WS_URL", cfg.network.ws_url);
  cfg.network.reconnect_ms= get_env_i("CHESSMATE_WS_RECONNECT_MS", cfg.network.reconnect_ms);
  return cfg;
}
}