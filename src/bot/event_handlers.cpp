#include "bot/event_handlers.hpp"

#include "config.hpp"
#include "core/gradual_editor.hpp"
#include "utils/speed.hpp"
#include "utils/text_utils.hpp"

#include <algorithm>
#include <cctype>
#include <format>
#include <string>

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
        bool isAccumulated = text.starts_with(ACCUMULATE_COMMAND);
        if (isAccumulated) {
            auto firstNonSpace = std::find_if(
                text.begin() + ACCUMULATE_COMMAND.size(), text.end(), [](char ch) { return !std::isspace(ch); });
            text = std::string(firstNonSpace, text.end());
        }
        std::string description;
        std::string savedText = userStorage.getText(query->from->id);
        if (text.empty() && savedText.empty()) {
            description = "Use " + SAVE_COMMAND + " in PM to save text first";
            text = "Empty message";
        } else {
            text = text.empty() ? savedText : text;
            description = "Query: " + text;
            if (isAccumulated)
                description = "(Accumulative mode) " + description;
        }

        for (size_t speed = 0; speed < SPEEDS.size(); ++speed) {
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
        const std::string savedText = userStorage.getText(chosenQuery->from->id);
        std::string text = chosenQuery->query.empty() ? savedText : chosenQuery->query;
        bool isAccumulated = text.starts_with(ACCUMULATE_COMMAND);
        if (isAccumulated)
            text = text.substr(ACCUMULATE_COMMAND.size());
        if (text.empty()) {
            text = "Empty message";
            if (!savedText.empty())
                text = savedText;
        }

        taskManager.startTask(chosenQuery->inlineMessageId,
                              std::jthread([&bot,
                                            inlineMessageId = chosenQuery->inlineMessageId,
                                            text = std::move(text),
                                            speed,
                                            isAccumulated,
                                            notify = [&taskManager, id = chosenQuery->inlineMessageId](const auto&) {
                                                taskManager.notifyFinished(id);
                                            }](std::stop_token stoken) {
                                  graduallyUpdateMessage(bot, inlineMessageId, text, speed, isAccumulated, notify);
                              }));
    });

    bot.getEvents().onCallbackQuery([&bot](TgBot::CallbackQuery::Ptr query) {
        if (query->data == "why_me") {
            bot.getApi().answerCallbackQuery(
                query->id, "Feel the pain of listening to voice and video messages 🎤😤🔊", true);
        }
    });
}
