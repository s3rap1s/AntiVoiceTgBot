#pragma once

#include <string>
#include <vector>

std::vector<std::string> splitText(const std::string& text);
std::vector<std::string> splitTextByWordsCount(const std::string& text, size_t wordsPerChunk);
std::string getInitialText(size_t speed, const std::string& text);
size_t toInteger(const std::string& str);
