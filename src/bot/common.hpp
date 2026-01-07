#pragma once

#include <vector>

#include <tgbot/types/InlineKeyboardButton.h>
#include <tgbot/types/InlineKeyboardMarkup.h>

inline TgBot::InlineKeyboardButton::Ptr createShowFullTextButton() {
    auto button = std::make_shared<TgBot::InlineKeyboardButton>();
    button->text = "📖 Show full text";
    button->callbackData = "show_full";
    return button;
}

inline TgBot::InlineKeyboardButton::Ptr createLisstenAgainButton() {
    auto button = std::make_shared<TgBot::InlineKeyboardButton>();
    button->text = "🔄 Listen again";
    button->callbackData = "relisten";
    return button;
}

inline TgBot::InlineKeyboardMarkup::Ptr createKeyboard() {
    TgBot::InlineKeyboardMarkup::Ptr keyboard = std::make_shared<TgBot::InlineKeyboardMarkup>();
    std::vector<TgBot::InlineKeyboardButton::Ptr> row;
    row.push_back(createShowFullTextButton());
    keyboard->inlineKeyboard.push_back(row);
    return keyboard;
}

