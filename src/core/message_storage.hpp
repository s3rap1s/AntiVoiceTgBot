#pragma once

#include "utils/types.hpp"

#include <cstddef>
#include <expected>
#include <shared_mutex>
#include <string>
#include <unordered_map>

#include <tgbot/types/Chat.h>
#include <tgbot/types/MessageId.h>

class MessageStorage {
  public:
    struct MessageInfo {
        std::string text;
        bool isAccumulated;
        size_t speed;
        UserId owner;
    };

    void saveMessage(
        const InlineMessageId& messageId, UserId userId, std::string_view text, bool isAccumulated, size_t speed);
    std::expected<MessageInfo, std::string> getMessage(const InlineMessageId& messageId) const;
    void forgetMessage(const InlineMessageId& messageId);

    void saveTempMessage(const InlineMessageId& messageId, std::string_view text);
    std::string getTempMessage(const InlineMessageId& messageId);

  private:
    mutable std::shared_mutex mainMutex;
    mutable std::shared_mutex tempMutex;
    std::unordered_map<InlineMessageId, MessageInfo> mainStorage;
    std::unordered_map<InlineMessageId, std::string> tempStorage;
};
