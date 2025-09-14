#include "infrastructure/config.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

namespace chess {

std::optional<Config> load_config(std::string_view config_path) noexcept {
    try {
        std::ifstream file(std::string(config_path));
        if (!file.is_open()) {
            std::cerr << "Warning: Could not open config file " << config_path 
                      << ", using default configuration\n";
            return std::nullopt;
        }

        nlohmann::json j;
        file >> j;

        Config config;
        
        if (j.contains("websocket")) {
            const auto& ws = j["websocket"];
            config.server_host = ws.value("host", config.server_host);
            config.server_port = ws.value("port", config.server_port);
            config.server_path = ws.value("path", config.server_path);
            config.ws_url = "ws://" + config.server_host + ":" + std::to_string(config.server_port) + config.server_path;
        }

        if (j.contains("audio")) {
            const auto& audio = j["audio"];
            config.enable_sound = audio.value("enabled", config.enable_sound);
        }

        if (j.contains("graphics")) {
            const auto& graphics = j["graphics"];
            config.enable_animations = graphics.value("animations", config.enable_animations);
            config.window_width = graphics.value("width", config.window_width);
            config.window_height = graphics.value("height", config.window_height);
            config.theme = graphics.value("theme", config.theme);
            config.fullscreen = graphics.value("fullscreen", config.fullscreen);
            config.vsync = graphics.value("vsync", config.vsync);
            config.target_fps = graphics.value("fps", config.target_fps);
        }

        if (j.contains("ai")) {
            const auto& ai = j["ai"];
            config.ai_depth = ai.value("depth", config.ai_depth);
        }

        if (j.contains("debug")) {
            const auto& debug = j["debug"];
            config.debug_mode = debug.value("enabled", config.debug_mode);
            config.log_level = debug.value("log_level", config.log_level);
            config.log_file = debug.value("log_file", config.log_file);
        }

        return config;
    } catch (const std::exception& e) {
        std::cerr << "Error loading config: " << e.what() << ", using default configuration\n";
        return std::nullopt;
    }
}

Config load_default_config() noexcept {
    return Config{};
}

bool save_config(const Config& config, std::string_view config_path) noexcept {
    try {
        nlohmann::json j;
        
        // Parse URL to extract components
        std::string url = config.ws_url;
        if (url.starts_with("ws://")) {
            url = url.substr(5); // Remove "ws://"
            size_t colon_pos = url.find(':');
            if (colon_pos != std::string::npos) {
                config.server_host = url.substr(0, colon_pos);
                size_t slash_pos = url.find('/', colon_pos);
                if (slash_pos != std::string::npos) {
                    config.server_port = std::stoi(url.substr(colon_pos + 1, slash_pos - colon_pos - 1));
                    config.server_path = url.substr(slash_pos);
                } else {
                    config.server_port = std::stoi(url.substr(colon_pos + 1));
                    config.server_path = "/game";
                }
            }
        }

        j["websocket"] = {
            {"host", config.server_host},
            {"port", config.server_port},
            {"path", config.server_path}
        };

        j["audio"] = {
            {"enabled", config.enable_sound}
        };

        j["graphics"] = {
            {"animations", config.enable_animations},
            {"width", config.window_width},
            {"height", config.window_height},
            {"theme", config.theme},
            {"fullscreen", config.fullscreen},
            {"vsync", config.vsync},
            {"fps", config.target_fps}
        };

        j["ai"] = {
            {"depth", config.ai_depth}
        };

        j["debug"] = {
            {"enabled", config.debug_mode},
            {"log_level", config.log_level},
            {"log_file", config.log_file}
        };

        std::ofstream file(std::string(config_path));
        if (!file.is_open()) {
            std::cerr << "Error: Could not open config file for writing: " << config_path << "\n";
            return false;
        }

        file << j.dump(2) << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error saving config: " << e.what() << "\n";
        return false;
    }
}

} // namespace chess
