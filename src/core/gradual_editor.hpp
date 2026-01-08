#pragma once

#include "core/message_storage.hpp"
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
                            MessageStorage& messageStorage,
                            NotifyCallback notifyFinished) noexcept;
