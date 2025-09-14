#pragma once

#include <string>
#include <string_view>
#include <optional>

namespace chess {

struct Config {
    std::string ws_url = "ws://localhost:8080/game";
    std::string server_host = "localhost";
    int server_port = 8080;
    std::string server_path = "/game";
    bool enable_sound = true;
    bool enable_animations = true;
    int ai_depth = 3;
    int window_width = 800;
    int window_height = 600;
    std::string theme = "classic";
    bool fullscreen = false;
    bool vsync = true;
    int target_fps = 60;
    bool debug_mode = false;
    std::string log_level = "info";
    std::string log_file = "chessmate.log";
};

[[nodiscard]] std::optional<Config> load_config(std::string_view config_path = "config.json") noexcept;
[[nodiscard]] Config load_default_config() noexcept;
bool save_config(const Config& config, std::string_view config_path = "config.json") noexcept;

} // namespace chess
