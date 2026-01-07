#include "commands.hpp"

#include "config.hpp"

#include <format>
#include <string>

void registerCommands(TgBot::Bot& bot, UserStorage& userStorage) {
    const std::string alias = bot.getApi().getMe()->username;
    bot.getEvents().onCommand(START_COMMAND.substr(1), [&bot, alias](TgBot::Message::Ptr message) {
        std::string param;
        if (message->text.size() > START_COMMAND.size() + 1) {
            param = message->text.substr(START_COMMAND.size() + 1);
        }
        if (param == "buyPrem") {
            bot.getApi().sendMessage(message->chat->id,
                                     "<b>💎 Premium version</b>\n\n"
                                     "• Ability to view full text without waiting\n"
                                     "• Prohibiting other premium users from viewing the full text\n\n"
                                     "Type /start to check out the bot's functionality",
                                     nullptr,
                                     0,
                                     nullptr,
                                     "HTML");
            return;
        }

        const std::string general_info = std::format("<b>🤖 Hi!</b> I'm a bot for gradual message sending.\n\n"
                                                     "<b>📝 How to use:</b>\n\n"
                                                     "<b>🤏 For short messages</b> (up to {} characters):\n"
                                                     "1. Type @{} text in any chat\n"
                                                     "2. Choose sending speed\n\n"
                                                     "<b>🫷 🫸 For long messages:</b>\n"
                                                     "1. Send {} command\n"
                                                     "2. Send your text (up to {} characters)\n"
                                                     "3. Use @{} in any chat\n"
                                                     "4. Choose sending speed\n",
                                                     MAX_QUERY_SIZE,
                                                     alias,
                                                     SAVE_COMMAND,
                                                     MAX_MESSAGE_SIZE,
                                                     alias);
        std::string speeds_info = "\n<b>⚡ Available speeds:</b>\n";
        for (const auto& speed : SPEEDS) {
            speeds_info +=
                std::format("{} - {} words per {:.1f} seconds\n", speed.title, speed.wordsPerChunk, speed.delay);
        }
        std::string commands_info = std::format("\n<b>📋 Commands:</b>\n"
                                                "{} - show this message\n"
                                                "{} - start saving long text\n"
                                                "{} - delete saved text\n",
                                                START_COMMAND,
                                                SAVE_COMMAND,
                                                CLEAR_COMMAND);
        std::string options_info = std::format("\n<b>🔧 Options:</b>\n"
                                               "{} - insert this at the beginning of the query so that "
                                               "the displayed text remains in the message",
                                               ACCUMULATE_COMMAND);
        const std::string fullText = general_info + speeds_info + commands_info + options_info;
        bot.getApi().sendMessage(message->chat->id, fullText, nullptr, 0, nullptr, "HTML");
    });

    bot.getEvents().onCommand(SAVE_COMMAND.substr(1), [&bot](TgBot::Message::Ptr message) {
        bot.getApi().sendMessage(message->chat->id,
                                 std::format("<b>💾 Text saving</b>\n\n"
                                             "Send me the text you want to save.\n"
                                             "After saving, you'll be able to use it in inline mode.\n\n"
                                             "<b>📏 Maximum size:</b> {} characters",
                                             MAX_MESSAGE_SIZE),
                                 nullptr,
                                 0,
                                 nullptr,
                                 "HTML");
    });

    bot.getEvents().onCommand(CLEAR_COMMAND.substr(1), [&bot, &userStorage](TgBot::Message::Ptr message) {
        userStorage.clearText(message->from->id);
        bot.getApi().sendMessage(message->chat->id,
                                 "<b>🗑 Done!</b>\n\n"
                                 "Saved text deleted.\n"
                                 "Use " +
                                     SAVE_COMMAND + " to save new text.",
                                 nullptr,
                                 0,
                                 nullptr,
                                 "HTML");
    });
}
