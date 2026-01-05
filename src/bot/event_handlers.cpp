#include "bot/event_handlers.hpp"

#include "core/gradual_editor.hpp"
#include "utils/speed.hpp"
#include "utils/text_utils.hpp"

#include <format>

void registerEventHandlers(TgBot::Bot& bot, UserStorage& userStorage, TaskManager& taskManager) {
    bot.getEvents().onAnyMessage([&bot, &userStorage](TgBot::Message::Ptr message) {
        if (message->text.empty() || message->text[0] == '/') {
            return;
        }
        userStorage.saveText(message->from->id, message->text);

        auto words = splitText(message->text);
        size_t wordCount = words.size();
        size_t charCount = message->text.size();

        std::string response = std::format("✅ Text saved!\n\n"
                                           "📊 Statistics:\n"
                                           "• Characters: {}\n"
                                           "• Words: {}\n\n"
                                           "Now use @{} in any chat to send it!",
                                           charCount,
                                           wordCount,
                                           bot.getApi().getMe()->username);

        bot.getApi().sendMessage(message->chat->id, response, nullptr, 0, nullptr, "HTML");
    });

    bot.getEvents().onInlineQuery([&bot, &userStorage](TgBot::InlineQuery::Ptr query) {
        TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);
        {
            TgBot::InlineKeyboardButton::Ptr button(new TgBot::InlineKeyboardButton);
            button->text = "Why me? 😩";
            button->callbackData = "why_me";
            std::vector<TgBot::InlineKeyboardButton::Ptr> row{button};
            keyboard->inlineKeyboard.push_back(row);
        }

        std::vector<TgBot::InlineQueryResult::Ptr> results;
        std::string text = query->query.empty() ? userStorage.getText(query->from->id) : query->query;
        std::string description;
        if (text.empty()) {
            description = "Use /save in PM to save text first";
            text = "Empty message";
        } else {
            description = "Query: " + text;
        }

        for (size_t speed = 0; speed < getSpeeds().size(); ++speed) {
            auto config = getSpeedInformation(speed);
            auto result = std::make_shared<TgBot::InlineQueryResultArticle>();
            result->id = std::to_string(speed);
            result->title = config.title;
            result->description = config.speedStr + " - " + description;
            auto inputContent = std::make_shared<TgBot::InputTextMessageContent>();
            inputContent->messageText = getInitialText(speed, text);
            result->inputMessageContent = inputContent;
            result->replyMarkup = keyboard;

            results.push_back(result);
        }
        bot.getApi().answerInlineQuery(query->id, results, 1, true);
    });

    bot.getEvents().onChosenInlineResult([&bot, &userStorage, &taskManager](
                                             TgBot::ChosenInlineResult::Ptr chosenQuery) {
        if (chosenQuery->inlineMessageId.empty() || chosenQuery->resultId.empty()) {
            return;
        }

        size_t speed = toInteger(chosenQuery->resultId);
        std::string fullText =
            chosenQuery->query.empty() ? userStorage.getText(chosenQuery->from->id) : chosenQuery->query;
        if (fullText.empty()) {
            fullText = "Empty message";
        }

        taskManager.startTask(chosenQuery->inlineMessageId,
                              std::jthread([&bot,
                                            inlineMessageId = chosenQuery->inlineMessageId,
                                            fullText = std::move(fullText),
                                            speed,
                                            notify = [&taskManager, id = chosenQuery->inlineMessageId](const auto&) {
                                                taskManager.notifyFinished(id);
                                            }](std::stop_token stoken) {
                                  graduallyUpdateMessage(bot, inlineMessageId, fullText, speed, notify);
                              }));
    });

    bot.getEvents().onCallbackQuery([&bot](TgBot::CallbackQuery::Ptr query) {
        if (query->data == "why_me") {
            bot.getApi().answerCallbackQuery(
                query->id, "Feel the pain of listening to voice and video messages 🎤😤🔊", true);
        }
    });
}
