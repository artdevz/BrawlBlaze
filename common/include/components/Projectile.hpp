#pragma once

#include <cstdint>

struct Projectile {
    uint32_t originID;

    Projectile(uint32_t originID) : originID(originID) {}
};