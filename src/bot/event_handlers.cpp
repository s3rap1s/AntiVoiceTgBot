#include "event_handlers.hpp"

#include "bot/common.hpp"
#include "config.hpp"
#include "core/gradual_editor.hpp"
#include "core/message_storage.hpp"
#include "utils/speed.hpp"
#include "utils/text_utils.hpp"
#include "utils/types.hpp"

#include <algorithm>
#include <cctype>
#include <format>
#include <ostream>
#include <string>
#include <thread>

void registerEventHandlers(TgBot::Bot& bot,
                           UserStorage& userStorage,
                           MessageStorage& messageStorage,
                           TaskManager& taskManager) {
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
        std::vector<TgBot::InlineQueryResult::Ptr> results;
        std::string text = query->query;
        if (query->query.empty()) {
            return;
        }
        bool isAccumulated = text.starts_with(ACCUMULATE_COMMAND);
        if (isAccumulated) {
            auto firstNonSpace = std::find_if(
                text.begin() + ACCUMULATE_COMMAND.size(), text.end(), [](char ch) { return !std::isspace(ch); });
            text = std::string(firstNonSpace, text.end());
        }
        std::cerr << "Text " << text;
        std::string description;
        if (text.empty()) {
            text = userStorage.getText(query->from->id);
        }
        std::cerr << "\ntext2 " << text << std::endl;
        if (!text.empty()) {
            description = "Query: " + text;
            if (isAccumulated)
                description = "(Accumulative mode) " + description;
            for (size_t speed = 0; speed < SPEEDS.size(); ++speed) {
                auto speedConfig = getSpeedInformation(speed);
                auto result = std::make_shared<TgBot::InlineQueryResultArticle>();
                result->id = std::to_string(speed);
                result->title = speedConfig.title;
                result->description = speedConfig.speedStr + " - " + description;
                auto inputContent = std::make_shared<TgBot::InputTextMessageContent>();
                inputContent->messageText = getInitialText(speed, text);
                result->inputMessageContent = inputContent;
                result->replyMarkup = createKeyboard();
                results.push_back(result);
            }
            bot.getApi().answerInlineQuery(query->id, results, 1, true);
        }
    });

    bot.getEvents().onChosenInlineResult(
        [&bot, &messageStorage, &taskManager](TgBot::ChosenInlineResult::Ptr chosenQuery) {
            if (chosenQuery->inlineMessageId.empty() || chosenQuery->resultId.empty()) {
                return;
            }
            size_t speed = toInteger(chosenQuery->resultId);
            std::string text = chosenQuery->query;
            bool isAccumulated = text.starts_with(ACCUMULATE_COMMAND);
            if (isAccumulated)
                text = text.substr(ACCUMULATE_COMMAND.size());
            InlineMessageId messageId = chosenQuery->inlineMessageId;
            messageStorage.saveMessage(messageId, chosenQuery->from->id, text, isAccumulated, speed);
            taskManager.startTask(messageId,
                                  std::jthread([&bot,
                                                messageId,
                                                text = std::move(text),
                                                speed,
                                                isAccumulated,
                                                notify = [&taskManager, messageId](const auto&) {
                                                    taskManager.notifyFinished(messageId);
                                                }](std::stop_token stoken) {
                                      graduallyUpdateMessage(bot, messageId, text, speed, isAccumulated, notify);
                                  }));
        });

    bot.getEvents().onCallbackQuery([&bot, &messageStorage, &userStorage, &taskManager](
                                        TgBot::CallbackQuery::Ptr query) {
        const std::string alias = bot.getApi().getMe()->username;
        if (query->data == "show_full") {
            auto result = messageStorage.getMessage(query->inlineMessageId);
            if (result.has_value()) {
                auto [text, speed, isAccumulated, owner] = result.value();
                if ((userStorage.isPremium(query->from->id) && !userStorage.isPremium(owner)) ||
                    owner == query->from->id)
                    bot.getApi().answerCallbackQuery(query->id, result.value().text, true);
                else {
                    bot.getApi().answerCallbackQuery(
                        query->id, "", true, std::format("t.me/{}?start={}", alias, "buyPrem"));
                }
            } else {
                bot.getApi().answerCallbackQuery(query->id, result.error(), false);
            }
        } else if (query->data == "relisten") {
            auto messageId = query->inlineMessageId;
            auto result = messageStorage.getMessage(messageId);
            if (result.has_value()) {
                auto [text, isAccumulated, speed, owner] = result.value();
                taskManager.startTask(messageId,
                                      std::jthread([&bot,
                                                    messageId,
                                                    text = std::move(text),
                                                    speed,
                                                    isAccumulated,
                                                    notify = [&taskManager, id = messageId](const auto&) {
                                                        taskManager.notifyFinished(id);
                                                    }](std::stop_token stoken) {
                                          graduallyUpdateMessage(bot, messageId, text, speed, isAccumulated, notify);
                                      }));
            } else {
                bot.getApi().answerCallbackQuery(query->id, result.error(), true);
            }
        }
    });
}
