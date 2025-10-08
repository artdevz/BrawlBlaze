#pragma once

struct Health {
    float current;
    float max;

    Health(float current = 100.0f, float max = 100.0f) : current(current), max(max) {}

    bool IsDead() const { return current <= 0.0f; }

    void MaxHeal() { current = max; }

    void Regenerate(float amount) {
        current += amount;
        if (current > max) current = max;
    }

    bool TakeDamage(float amount) {
        current -= amount;
        if (current <= 0.0f) {
            current = 0.0f;
            return true;
        }
        return false;
    }
};