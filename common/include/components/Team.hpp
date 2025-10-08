#pragma once

#include <cstdint>

enum class TeamColor : uint8_t {
    Blue = 1,
    Red,
    None
};

struct Team {
    TeamColor color;

    Team(TeamColor color = TeamColor::None) : color(color) {}
};