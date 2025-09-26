#pragma once

#include <cstdint>

enum class ClientPacketType : uint8_t {
    Connect = 1,
    Input = 2,
    Message = 3
};

enum class ServerPacketType : uint8_t {
    Connect = 1,
    EntityState = 2
};