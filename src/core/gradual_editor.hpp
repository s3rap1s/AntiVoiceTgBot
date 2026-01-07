#pragma once

#include <functional>
#include <string>

#include <tgbot/tgbot.h>

using NotifyCallback = std::function<void(const std::string&)>;

void graduallyUpdateMessage(TgBot::Bot& bot,
                            const std::string& inlineMessageId,
                            const std::string& fullText,
                            size_t speed,
                            bool isAccumulated,
                            NotifyCallback notifyFinished) noexcept;
