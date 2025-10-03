#pragma once

#include <cstdint>

enum class ClientPacketType : uint8_t {
    Connect = 1,
    Input = 2,
    Message = 3
};

enum class ServerPacketType : uint8_t {
    Init = 1,
    EntityState = 2,
    Add = 3,
    Remove = 4,
    CombatStats = 5
};

#pragma pack(push, 1)

// ================ //
// CLIENT -> SERVER //
// ================ //

struct ClientPacketHeader {
    ClientPacketType type;
};

struct ConnectPayload {
    char nickname[32];
};

struct InputPayload {
    uint32_t playerID;
    uint32_t inputSequence;
    int8_t x, y;
    bool isMouseUsed = false;
    float targetX, targetY;
    float deltaTime;
};

struct MessagePayload {
    char message[128];
};

// ================ //
// SERVER -> CLIENT //
// ================ //

struct ServerPacketHeader {
    ServerPacketType type;
};

struct InitPayload {
    uint32_t entityID;
    char nickname[32];
    float x, y;
    float hp, maxHP;
    uint8_t team;
};

struct EntityStatePayload {
    uint32_t entityID;
    float x, y;
    float hp;
    uint32_t lastProcessedInput;
};

struct AddEntityPayload {
    uint32_t entityID;
    uint16_t type;
    float x, y;
    float hp, maxHP;
    char nickname[32];
    uint8_t team;
};

struct RemoveEntityPayload {
    uint32_t entityID;
};

struct CombatStatsPaylod {
    uint32_t entityID;
    uint16_t kills;
    uint16_t deaths;
    uint16_t assists;
};

#pragma pack(pop)