#include "bot/commands.hpp"
#include "bot/event_handlers.hpp"
#include "config.hpp"
#include "core/message_storage.hpp"
#include "core/task_manager.hpp"
#include "core/user_storage.hpp"

#include <iostream>
#include <tgbot/tgbot.h>

int main() {
    try {
        TgBot::Bot bot(API_TOKEN);
        bot.getApi().deleteWebhook(true);

        UserStorage userStorage;
        TaskManager taskManager;
        MessageStorage messageStorage;

        registerCommands(bot, userStorage);
        registerEventHandlers(bot, userStorage, messageStorage, taskManager);

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
