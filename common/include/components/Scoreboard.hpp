#pragma once

#include <cstdint>

struct Scoreboard {
    uint8_t blueScore;
    uint8_t redScore;

    Scoreboard(uint8_t blueScore = 0, uint8_t redScore = 0) : blueScore(blueScore), redScore(redScore) {}
};