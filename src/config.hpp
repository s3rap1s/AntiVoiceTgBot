#pragma once

#include "utils/speed.hpp"

#include <string>
#include <vector>

inline constexpr size_t MAX_QUERY_SIZE = 256;
inline constexpr size_t MAX_MESSAGE_SIZE = 4096;
inline constexpr size_t DEFAULT_SPEED = 0;

inline const std::string API_TOKEN = std::getenv("TELEGRAM_BOT_TOKEN");

const std::vector<SpeedInformation> SPEEDS = {{5.0, 1, "🐌 Very Slow (Torture Mode)", "5s/word"},
                                              {3.0, 2, "🐢 Slow", "3s/2words"},
                                              {1.5, 3, "🦮 Medium", "1.5s/3words"},
                                              {1.0, 5, "🐆 Fast", "1s/5words"}};

inline constexpr std::string START_COMMAND = "/start";
inline constexpr std::string SAVE_COMMAND = "/save";
inline constexpr std::string CLEAR_COMMAND = "/clear";
inline constexpr std::string ACCUMULATE_COMMAND = "/a";

