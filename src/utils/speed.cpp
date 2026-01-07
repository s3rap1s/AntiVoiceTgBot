#include "speed.hpp"

#include "config.hpp"

SpeedInformation getSpeedInformation(size_t speed) {
    if (speed >= SPEEDS.size()) {
        speed = DEFAULT_SPEED;
    }
    return SPEEDS[speed];
}
