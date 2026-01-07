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

    void
    saveMessage(InlineMessageId& messageId, UserId userId, const std::string& text, bool isAccumulated, size_t speed);
    std::expected<MessageInfo, std::string> getMessage(InlineMessageId& messageId) const;
    void forgetMessage(InlineMessageId& messageId);

  private:
    mutable std::shared_mutex mutex;
    std::unordered_map<InlineMessageId, MessageInfo> storage;
};
