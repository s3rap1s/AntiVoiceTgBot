#pragma once

#include <string>

inline constexpr size_t MAX_MESSAGE_SIZE = 4096;
inline constexpr size_t DEFAULT_SPEED = 0;

inline const std::string API_TOKEN = std::getenv("TELEGRAM_BOT_TOKEN");
