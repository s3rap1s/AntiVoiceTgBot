#pragma once

#include <string>
#include <string_view>
#include <vector>

std::vector<std::string> splitText(std::string_view text);
std::vector<std::string> splitTextByWordsCount(std::string_view text, size_t wordsPerChunk, bool isAccumulated);
std::string getInitialText(size_t speed, std::string_view text);
size_t toInteger(std::string_view str);
