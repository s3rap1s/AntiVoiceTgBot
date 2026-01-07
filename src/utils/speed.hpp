#pragma once

#include <string>

struct SpeedInformation {
    double delay;
    size_t wordsPerChunk;
    std::string title;
    std::string speedStr;
};

SpeedInformation getSpeedInformation(size_t speed);
