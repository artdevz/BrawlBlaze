#pragma once

#include <cmath>
#include <cstdint>

struct Armour {
    uint16_t value;

    Armour(uint16_t value = 0) : value(value) {}

    void AddValue(uint16_t amount) {
        value += amount;
    }

    void RemoveValue(uint16_t amount) {
        if (amount > value) {
            value = 0;
            return;
        }
        value -= amount;
    }

    float ReduceDamage(float damage) {
        return damage - sqrt(value);
    }
};