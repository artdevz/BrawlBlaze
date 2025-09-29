#pragma once

struct Velocity {
    float maxSpeed;
    float dx, dy;

    Velocity(float maxSpeed, float dx = 0.0f, float dy = 0.0f) : maxSpeed(maxSpeed), dx(dx), dy(dy) {}
};