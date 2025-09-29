#pragma once

#include <cstdint>

enum class TeamColor : uint8_t {
    None,
    Blue,
    Red
};

struct Team {
    TeamColor color;

    Team(TeamColor color = TeamColor::None) : color(color) {}
};