#include "user_storage.hpp"

#include "config.hpp"

#include <mutex>
#include <shared_mutex>
#include <string>

void UserStorage::makePremium(UserId userId, size_t subscriptionDuration) {
    std::unique_lock lock(premuimMutex);
    premiumUsers[userId] = true;
}

bool UserStorage::isPremium(UserId userId) const {
    std::shared_lock lock(premuimMutex);
    auto it = premiumUsers.find(userId);
    return (it != premiumUsers.end()) ? it->second : false;
}

void UserStorage::saveText(UserId userId, const std::string& text) {
    std::unique_lock lock(textMutex);
    if (text.size() > MAX_MESSAGE_SIZE)
        textStorage[userId] = text.substr(MAX_MESSAGE_SIZE - 3) + "...";
    else
        textStorage[userId] = text;
}

std::string UserStorage::getText(UserId userId) const {
    std::shared_lock lock(textMutex);
    auto it = textStorage.find(userId);
    return (it != textStorage.end()) ? it->second : "";
}

void UserStorage::clearText(UserId userId) {
    std::unique_lock lock(textMutex);
    textStorage.erase(userId);
}
