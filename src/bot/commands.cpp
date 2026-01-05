#include "bot/commands.hpp"
#include <format>

void registerCommands(TgBot::Bot& bot, UserStorage& userStorage) {
    const std::string& tg = bot.getApi().getMe()->username;
    bot.getEvents().onCommand("start", [&bot, tg](TgBot::Message::Ptr message) {
        bot.getApi().sendMessage(message->chat->id,
                                 std::format("🤖 Hi! I'm a bot for gradual message sending.\n\n"
                                             "📝 How to use:\n\n"
                                             "For short messages (up to 256 characters):\n"
                                             "1. Type @{} text in any chat\n"
                                             "2. Choose sending speed\n\n"
                                             "For long messages:\n"
                                             "1. Send /save command\n"
                                             "2. Send your text (up to 4096 characters)\n"
                                             "3. Use @{} in any chat\n"
                                             "4. Choose sending speed\n\n"
                                             "⚡ Available speeds:\n"
                                             "🐌 Very Slow — Word per 5 seconds (tortoise mode)\n"
                                             "🐢 Slow — 2 words per 2 second\n"
                                             "🚶 Medium — 3 words per 1.5 second\n"
                                             "🏃 Fast — 5 words per second\n\n"
                                             "📋 Commands:\n"
                                             "/save — start saving long text\n"
                                             "/clear — delete saved text\n"
                                             "/start — show this message",
                                             tg,
                                             tg),
                                 nullptr,
                                 0,
                                 nullptr,
                                 "HTML");
    });

    bot.getEvents().onCommand("save", [&bot](TgBot::Message::Ptr message) {
        bot.getApi().sendMessage(message->chat->id,
                                 "💾 Text saving\n\n"
                                 "Send me the text you want to save.\n"
                                 "After saving, you'll be able to use it in inline mode.\n\n"
                                 "📏 Maximum size: 4096 characters",
                                 nullptr,
                                 0,
                                 nullptr,
                                 "HTML");
    });

    bot.getEvents().onCommand("clear", [&bot, &userStorage](TgBot::Message::Ptr message) {
        userStorage.clearText(message->from->id);
        bot.getApi().sendMessage(message->chat->id,
                                 "🗑 Done!\n\nSaved text deleted.\nUse /save to save new text.",
                                 nullptr,
                                 0,
                                 nullptr,
                                 "HTML");
    });
}
