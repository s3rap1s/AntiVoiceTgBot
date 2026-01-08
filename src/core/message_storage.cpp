#include "message_storage.hpp"
#include "utils/types.hpp"

#include <cstddef>
#include <expected>
#include <mutex>
#include <shared_mutex>
#include <string>

void MessageStorage::saveMessage(
    const InlineMessageId& id, UserId userId, std::string_view text, bool isAccumulated, size_t speed) {
    std::unique_lock lock(mainMutex);
    mainStorage[id] = {std::string(text), isAccumulated, speed, userId};
}

std::expected<MessageStorage::MessageInfo, std::string> MessageStorage::getMessage(const InlineMessageId& id) const {
    std::shared_lock lock(mainMutex);
    auto it = mainStorage.find(id);
    if (it == mainStorage.end())
        return std::unexpected<std::string>("Message was deleted from server.");
    return it->second;
}

void MessageStorage::forgetMessage(const InlineMessageId& id) {
    std::unique_lock lock(mainMutex);
    mainStorage.erase(id);
}

void MessageStorage::saveTempMessage(const InlineMessageId& id, std::string_view text) {
    std::unique_lock lock(tempMutex);
    tempStorage[id] = text;
}

std::string MessageStorage::getTempMessage(const InlineMessageId& id) {
    std::unique_lock lock(tempMutex);
    auto it = tempStorage.find(id);
    std::string text = it->second;
    tempStorage.erase(it);
    return text;
}
