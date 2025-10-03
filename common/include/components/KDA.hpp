#pragma once

#include <cstdint>

struct KDA {
    uint32_t kills;
    uint32_t deaths;
    uint32_t assists;

    KDA(uint32_t kills = 0, uint32_t deaths = 0, uint32_t assists = 0) : kills(kills), deaths(deaths), assists(assists) {}
};