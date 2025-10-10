#pragma once

#include <cstdint>

struct Inventory {
    bool flag;
    uint32_t flagID; // Depois seria bom fazer um pair

    Inventory(bool flag = false) : flag(flag) {}
};