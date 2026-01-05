#pragma once

#include "core/task_manager.hpp"
#include "core/user_storage.hpp"

#include <tgbot/tgbot.h>

void registerEventHandlers(TgBot::Bot& bot, UserStorage& userStorage, TaskManager& taskManager);
