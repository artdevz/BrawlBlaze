#pragma once

#include <cstdint>

struct Cooldown {
    float cooldown;
    float remain;

    Cooldown(float cooldown = 1000.0f) : cooldown(cooldown), remain(0.0f) {}

    bool IsReady() const {
        return remain == 0;
    }

    bool ReduceCooldown(uint64_t amount) {
        remain -= amount;
        if (remain <= 0.0f) {
            remain = 0.0f;
            return true;
        }
        return false;
    }

    bool Reset() {
        if (IsReady()) {
            remain = cooldown;
            return true;
        }
        return false;
    }
};