#pragma once

#include "utils/types.hpp"

#include <shared_mutex>
#include <string>
#include <unordered_map>

class UserStorage {
  public:
    void saveText(UserId userId, const std::string& text);
    std::string getText(UserId userId) const;
    void clearText(UserId userId);

    void makePremium(UserId userId, size_t subscriptionDuration);
    bool isPremium(UserId userId) const;

  private:
    mutable std::shared_mutex textMutex;
    mutable std::shared_mutex premuimMutex;
    std::unordered_map<UserId, std::string> textStorage;
    std::unordered_map<UserId, bool> premiumUsers;
};
