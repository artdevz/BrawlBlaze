#pragma once

#include <cstdint>

struct Scoreboard {
    uint8_t blueKills;
    uint8_t redKills;
    uint8_t blueScore;
    uint8_t redScore;

    Scoreboard(uint8_t blueKills = 0, uint8_t redKills = 0, uint8_t blueScore = 0, uint8_t redScore = 0) : blueKills(blueKills), redKills(redKills), blueScore(blueScore), redScore(redScore) {}
};