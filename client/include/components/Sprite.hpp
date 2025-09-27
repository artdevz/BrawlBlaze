#pragma once

#include <string>
#include <raylib.h>

struct Sprite {
    std::string id;

    Sprite(const std::string& id) : id(id) {}
};