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
#include <cstddef>
#include <format>
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
        if (message->text == "makeMePremiumUser") {
            userStorage.makePremium(message->from->id, 1);
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
        std::string savedText;
        if (query->query.empty()) {
            savedText = userStorage.getText(query->from->id);
            if (savedText.empty())
                return;
        }
        bool isAccumulated = text.starts_with(ACCUMULATE_COMMAND);
        if (isAccumulated) {
            auto firstNonSpace = std::find_if(
                text.begin() + ACCUMULATE_COMMAND.size(), text.end(), [](char ch) { return !std::isspace(ch); });
            text = std::string(firstNonSpace, text.end());
        }
        if (text.empty()) {
            text = userStorage.getText(query->from->id);
        }

        std::string description;
        if (!text.empty()) {
            description = "Query: " + text;
            if (isAccumulated)
                description = "(Accumulative mode) " + description;
            for (size_t speed = 0; speed < SPEEDS.size(); ++speed) {
                auto speedConfig = getSpeedInformation(speed);
                auto result = std::make_shared<TgBot::InlineQueryResultArticle>();
                result->id = std::format("{}-{}", speed, isAccumulated);
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
        [&bot, &userStorage, &messageStorage, &taskManager](TgBot::ChosenInlineResult::Ptr chosenQuery) {
            std::string queryId = chosenQuery->resultId;
            size_t separatorPos = queryId.find("-");
            size_t speed = toInteger(queryId.substr(0, separatorPos));
            size_t isAccumulated = queryId.substr(separatorPos + 1) == "true" ? true : false;
            std::string text = chosenQuery->query;
            if (isAccumulated)
                text = text.substr(ACCUMULATE_COMMAND.size());
            if (text.empty()) {
                text = userStorage.getText(chosenQuery->from->id);
            }
            const InlineMessageId messageId = chosenQuery->inlineMessageId;
            messageStorage.saveMessage(messageId, chosenQuery->from->id, text, isAccumulated, speed);
            taskManager.startTask(messageId,
                                  std::jthread([&bot,
                                                messageId,
                                                text = std::move(text),
                                                speed,
                                                isAccumulated,
                                                &messageStorage,
                                                notify = [&taskManager, messageId](const auto&) {
                                                    taskManager.notifyFinished(messageId);
                                                }](std::stop_token stoken) {
                                      graduallyUpdateMessage(
                                          bot, messageId, text, speed, isAccumulated, messageStorage, notify);
                                  }));
        });

    bot.getEvents().onCallbackQuery(
        [&bot, &messageStorage, &userStorage, &taskManager](TgBot::CallbackQuery::Ptr query) {
            const std::string alias = bot.getApi().getMe()->username;
            if (query->data == "show_full") {
                auto result = messageStorage.getMessage(query->inlineMessageId);
                if (result.has_value()) {
                    auto [text, speed, isAccumulated, owner] = result.value();
                    bool isOwner = owner == query->from->id;
                    bool isCallbackerPremium = userStorage.isPremium(query->from->id);
                    bool isOwnerPremium = userStorage.isPremium(owner);
                    if ((isCallbackerPremium && !isOwnerPremium) || isOwner)
                        bot.getApi().answerCallbackQuery(query->id, result.value().text, true);
                    else {
                        if (!isCallbackerPremium)
                            bot.getApi().answerCallbackQuery(
                                query->id, "", true, std::format("t.me/{}?start={}", alias, "buyPrem"));
                        else
                            bot.getApi().answerCallbackQuery(query->id, "The sender is also a 💎Premium user");
                    }
                } else {
                    bot.getApi().answerCallbackQuery(query->id, result.error(), false);
                }
            } else if (query->data == "relisten") {
                const InlineMessageId messageId = query->inlineMessageId;
                auto result = messageStorage.getMessage(messageId);
                if (result.has_value()) {
                    auto [text, isAccumulated, speed, owner] = result.value();
                    taskManager.startTask(messageId,
                                          std::jthread([&bot,
                                                        messageId,
                                                        text = std::move(text),
                                                        speed,
                                                        isAccumulated,
                                                        &messageStorage,
                                                        notify = [&taskManager, id = messageId](const auto&) {
                                                            taskManager.notifyFinished(id);
                                                        }](std::stop_token stoken) {
                                              graduallyUpdateMessage(
                                                  bot, messageId, text, speed, isAccumulated, messageStorage, notify);
                                          }));
                } else {
                    bot.getApi().answerCallbackQuery(query->id, result.error(), true);
                }
            }
        });
}
