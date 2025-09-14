#include "infrastructure/config.hpp"
#include <cstdlib>
#include <fstream>
#include <nlohmann/json.hpp>

using nlohmann::json;

namespace cm {
namespace {
  std::string getenv_str(const char* key, const std::string& def) {
    if (const char* v = std::getenv(key)) return std::string{v};
    return def;
  }
  int getenv_int(const char* key, int def) {
    if (const char* v = std::getenv(key)) return std::atoi(v);
    return def;
  }
}

Config load_config(std::optional<std::string> path) {
  Config cfg{};
  if (path && !path->empty()) {
    try {
      std::ifstream in(*path);
      if (in) {
        json j; in >> j;
        if (auto n = j.find("network"); n != j.end()) {
          cfg.network.ws_url = (*n).value("ws_url", cfg.network.ws_url);
          cfg.network.reconnect_ms = (*n).value("reconnect_ms", cfg.network.reconnect_ms);
        }
        if (auto u = j.find("ui"); u != j.end()) {
          cfg.ui.window_width  = (*u).value("window_width", cfg.ui.window_width);
          cfg.ui.window_height = (*u).value("window_height", cfg.ui.window_height);
          cfg.ui.vsync         = (*u).value("vsync", cfg.ui.vsync);
        }
      }
    } catch (...) { /* ignore, keep defaults */ }
  }
  // Env overrides
  cfg.network.ws_url     = getenv_str("CHESSMATE_WS_URL", cfg.network.ws_url);
  cfg.network.reconnect_ms = getenv_int("CHESSMATE_WS_RECONNECT_MS", cfg.network.reconnect_ms);
  return cfg;
}
} // namespace cm