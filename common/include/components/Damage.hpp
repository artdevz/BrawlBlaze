#pragma once

struct Damage {
    float value;

    Damage(float value = 0.0f) : value(value) {}

    void AddDamage(float amount) {
        if (amount > 0) value += amount;
    }

    void RemoveDamage(float amount) {
        if (amount > 0) {
            value -= amount;
            if (value < 0) value = 0;
        }
    }

};