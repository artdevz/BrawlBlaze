#pragma once

#include <cstdint>

struct Wallet {
    uint16_t silver;
    uint16_t gold;

    Wallet(uint16_t silver = 0, uint16_t gold = 0) : silver(silver), gold(gold) {}

    void AddSilver(uint16_t amount) { silver += amount; }

    void AddGold(uint16_t amount) { gold += amount; }
};