#pragma once

#include <string>
#include <vector>

struct SpeedInformation {
    double delay;
    size_t wordsPerChunk;
    std::string title;
    std::string speedStr;
};

const std::vector<SpeedInformation>& getSpeeds();
SpeedInformation getSpeedInformation(size_t speed);
