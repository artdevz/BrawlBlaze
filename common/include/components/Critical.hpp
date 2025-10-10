#pragma once

#include <cstdint>

struct Critical {
    uint8_t chance;
    float multiplier;

    Critical(uint8_t chance = 0, float multiplier = 1.5f) : chance(chance), multiplier(multiplier) {}

    void AddChance(uint8_t amount) {
        chance += amount;
        if (chance > 100) chance = 100;
    }

    void RemoveChance(uint8_t amount) {
        chance -= amount;
        if (chance > 100) chance = 0; // Checa Underflow
    }
};