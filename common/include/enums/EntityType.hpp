#pragma once

#include <cstdint>

enum class EntityType : uint16_t {
    None = 0,
    Player,
    FloorTile,
    Projectile,
    Tower
};