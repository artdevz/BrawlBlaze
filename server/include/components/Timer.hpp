#pragma once

#include <cstdint>

struct Timer {
    uint64_t time;

    Timer() : time(0) {}

    uint8_t GetMinutes() {
        return static_cast<uint8_t>(time/60);
    }

    uint8_t GetSeconds() {
        return static_cast<uint8_t>(time%60);
    }
};