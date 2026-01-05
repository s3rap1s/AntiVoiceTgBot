#include "utils/text_utils.hpp"

#include "config.hpp"
#include "utils/speed.hpp"

#include <sstream>

size_t toInteger(const std::string& str) {
    std::stringstream ss(str);
    size_t res{};
    ss >> res;
    return res;
}

std::vector<std::string> splitText(const std::string& text) {
    std::vector<std::string> words;
    std::stringstream ss(text);
    std::string word;
    while (ss >> word) {
        words.push_back(word);
    }
    return words;
}

std::string getInitialText(size_t speed, const std::string& text) {
    std::stringstream ss(text);
    std::string initialText;
    std::string word;
    size_t wordsToShow = getSpeedInformation(speed).wordsPerChunk;

    while (ss >> word && wordsToShow-- > 0) {
        initialText += word + " ";
    }
    return initialText;
}

std::vector<std::string> splitTextByWordsCount(const std::string& text, size_t wordsPerChunk) {
    auto words = splitText(text);
    std::vector<std::string> chunks;

    if (words.empty()) {
        return {};
    }

    size_t currentWordIndex = 0;
    while (currentWordIndex < words.size()) {
        size_t endIndex = std::min(currentWordIndex + wordsPerChunk, words.size());

        std::string chunk;
        if (!chunks.empty())
            chunk = chunks.back() + " ";

        for (size_t i = currentWordIndex; i < endIndex; ++i) {
            if (i > currentWordIndex) {
                chunk += " ";
            }
            chunk += words[i];
        }

        if (chunk.size() > MAX_MESSAGE_SIZE) {
            chunk = chunk.substr(0, MAX_MESSAGE_SIZE - 3) + "...";
            chunks.push_back(chunk);
            break;
        }

        chunks.push_back(chunk);
        currentWordIndex = endIndex;
    }
    return chunks;
}
