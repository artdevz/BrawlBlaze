#pragma once

#include <cstdint>

struct Dead {
    float respawnTime;

    Dead(float respawnTime = 5000.0f) : respawnTime(respawnTime) {}

    bool ReduceRespawnTime(float amount) {
        respawnTime -= amount;
        if (respawnTime <= amount) {
            respawnTime = 0.0f;
            return true;
        }
        return false;
    }
};