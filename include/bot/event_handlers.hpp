#pragma once

#include "core/message_storage.hpp"
#include "core/task_manager.hpp"
#include "core/user_storage.hpp"

#include <tgbot/tgbot.h>

void registerEventHandlers(TgBot::Bot& bot,
                           UserStorage& userStorage,
                           MessageStorage& messageStorage,
                           TaskManager& taskManager);
