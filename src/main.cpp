#include "bot/commands.hpp"
#include "bot/event_handlers.hpp"
#include "config.hpp"
#include "core/task_manager.hpp"
#include "core/user_storage.hpp"

#include <iostream>
#include <tgbot/tgbot.h>

int main() {
    try {
        TgBot::Bot bot(API_TOKEN);

        UserStorage userStorage;
        TaskManager taskManager;

        registerCommands(bot, userStorage);
        registerEventHandlers(bot, userStorage, taskManager);

        std::cout << "Bot started!" << std::endl;
        TgBot::TgLongPoll longPoll(bot);

        while (true) {
            longPoll.start();
        }
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
