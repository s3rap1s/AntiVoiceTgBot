#pragma once

#include "utils/types.hpp"
#include <functional>
#include <string>

#include <string_view>
#include <tgbot/tgbot.h>

using NotifyCallback = std::function<void(const std::string&)>;

void graduallyUpdateMessage(TgBot::Bot& bot,
                            const InlineMessageId& inlineMessageId,
                            std::string_view fullText,
                            size_t speed,
                            bool isAccumulated,
                            NotifyCallback notifyFinished) noexcept;
