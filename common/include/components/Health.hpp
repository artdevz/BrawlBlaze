#pragma once

struct Health {
    float current;
    float max;

    Health(float current = 100.0f, float max = 100.0f) : current(current), max(max) {}
};