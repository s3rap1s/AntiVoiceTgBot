#pragma once

#include "core/user_storage.hpp"

#include <tgbot/tgbot.h>

void registerCommands(TgBot::Bot& bot, UserStorage& userStorage);
