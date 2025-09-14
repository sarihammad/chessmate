#pragma once
#include <string>
#include <nlohmann/json.hpp>

namespace cm::proto {
constexpr int kVersion = 1;

inline nlohmann::json make_move(const std::string& from, const std::string& to, const std::string& uuid) {
  return {{"v", kVersion}, {"type","move"}, {"from",from}, {"to",to}, {"uuid",uuid}};
}

inline nlohmann::json make_state(const std::string& fen, const std::string& turn) {
  return {{"v", kVersion}, {"type","state"}, {"fen",fen}, {"turn",turn}};
}

inline nlohmann::json make_error(const std::string& code, const std::string& msg) {
  return {{"v", kVersion}, {"type","error"}, {"code",code}, {"msg",msg}};
}

inline bool is_valid(const nlohmann::json& j) {
  if (!j.contains("v") || !j["v"].is_number_integer()) return false;
  if ((int)j["v"] != kVersion) return false;
  if (!j.contains("type") || !j["type"].is_string()) return false;
  return true;
}

inline std::string get_type(const nlohmann::json& j) {
  if (j.contains("type") && j["type"].is_string()) {
    return j["type"];
  }
  return "";
}
} // namespace cm::proto
