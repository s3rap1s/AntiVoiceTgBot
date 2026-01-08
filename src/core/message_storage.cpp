#include "message_storage.hpp"
#include "utils/types.hpp"

#include <cstddef>
#include <expected>
#include <mutex>
#include <shared_mutex>
#include <string>

void MessageStorage::saveMessage(
    const InlineMessageId& id, UserId userId, const std::string_view text, bool isAccumulated, size_t speed) {
    std::unique_lock lock(mutex);
    storage[id] = {std::string(text), isAccumulated, speed, userId};
}

std::expected<MessageStorage::MessageInfo, std::string> MessageStorage::getMessage(const InlineMessageId& id) const {
    std::shared_lock lock(mutex);
    auto it = storage.find(id);
    if (it == storage.end())
        return std::unexpected<std::string>("Message was deleted from server.");
    return it->second;
}

void MessageStorage::forgetMessage(const InlineMessageId& id) {
    std::unique_lock lock(mutex);
    storage.erase(id);
}
