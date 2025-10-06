#pragma once

#include <cstdint>

struct Dead {
    uint16_t respawnTime;

    Dead(uint16_t respawnTime = 3000) : respawnTime(respawnTime) {}

    bool ReduceRespawnTime(uint16_t amount) {
        if (respawnTime <= amount) return true;
        respawnTime -= amount;
        return false;
    }
};