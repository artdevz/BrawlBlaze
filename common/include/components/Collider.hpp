#pragma once

#include "components/Position.hpp"

struct Collider {
    float width;
    float height;

    Collider(float width, float height) : width(width), height(height) {}

    bool Intersects(const Position& aPos, const Collider& other, const Position& otherPos) const {
        return !(
            aPos.x + width <= otherPos.x        ||
            aPos.x >= otherPos.x + other.width  ||
            aPos.y + height <= otherPos.y       ||
            aPos.y >= otherPos.y + other.height
        );
    }
};