#include "gradual_editor.hpp"

#include "bot/common.hpp"
#include "utils/speed.hpp"
#include "utils/text_utils.hpp"
#include "utils/types.hpp"

#include <iostream>
#include <string_view>
#include <thread>
#include <vector>

#include <tgbot/types/InlineKeyboardButton.h>
#include <tgbot/types/InlineKeyboardMarkup.h>

void graduallyUpdateMessage(TgBot::Bot& bot,
                            const InlineMessageId& inlineMessageId,
                            std::string_view fullText,
                            size_t speed,
                            bool isAccumulated,
                            NotifyCallback notifyFinished) noexcept try {
    auto speedInfo = getSpeedInformation(speed);
    auto chunks = splitTextByWordsCount(fullText, speedInfo.wordsPerChunk, isAccumulated);

    auto keyboard = createKeyboard();

    for (size_t i = 0; i < chunks.size(); ++i) {
        std::string displayText = chunks[i];
        if (i + 2 < chunks.size()) {
            displayText += "...";
        }

        try {
            if (i == chunks.size() - 1 && !isAccumulated)
                keyboard->inlineKeyboard.push_back({createLisstenAgainButton()});
            bot.getApi().editMessageText(displayText, 0, 0, inlineMessageId, "HTML", nullptr, keyboard);
        } catch (const std::exception& e) {
            if (!std::string(e.what()).starts_with("Bad Request: message is not modified")) {
                std::cerr << "Error editing message: " << e.what() << std::endl;
                break;
            }
        }

        if (i + 1 < chunks.size()) {
            std::this_thread::sleep_for(std::chrono::duration<double>(speedInfo.delay));
        }
    }

    if (notifyFinished) {
        notifyFinished(inlineMessageId);
    }

} catch (const std::exception& e) {
    std::cerr << "Unhandled exception: " << e.what() << std::endl;
} catch (...) {
    std::cerr << "Unhandled unknown exception" << std::endl;
}
