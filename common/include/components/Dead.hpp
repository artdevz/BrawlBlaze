#pragma once

#include <cstdint>

struct Dead {
    uint16_t respawnTime;

    Dead(uint16_t respawnTime = 3000) : respawnTime(respawnTime) {}
};