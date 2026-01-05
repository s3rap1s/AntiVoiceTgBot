#include "utils/speed.hpp"

#include "config.hpp"

namespace {
const std::vector<SpeedInformation> SPEEDS = {{5.0, 1, "🐌 Very Slow (Tortoise Mode)", "5s/word"},
                                              {3.0, 2, "🐢 Slow", "3s/2words"},
                                              {1.5, 3, "🦮 Medium", "1.5s/3words"},
                                              {1.0, 5, "🐆 Fast", "1s/5words"}};
}

const std::vector<SpeedInformation>& getSpeeds() {
    return SPEEDS;
}

SpeedInformation getSpeedInformation(size_t speed) {
    if (speed >= SPEEDS.size()) {
        speed = DEFAULT_SPEED;
    }
    return SPEEDS[speed];
}
