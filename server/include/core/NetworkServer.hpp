#pragma once

#include <cstdint>
#include <cstring>
#include <iostream>
#include <unordered_map>
#include <vector>

#include "../../../common/include/Packets.hpp"

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
#endif

struct Packet {
    uint8_t type;
    std::vector<uint8_t> payload;
    size_t size;
    sockaddr_in addr;
};

class Server {

public:
    Server();
    ~Server();

    bool Start(uint16_t port);
    void Stop();

    template<typename Payload>
    void Send(const ServerPacketType type, const Payload& payload, const sockaddr_in& clientAddr) {
        ServerPacketHeader header{};
        header.type = type;

        uint8_t buffer[sizeof(ServerPacketHeader) + sizeof(Payload)];
        std::memcpy(buffer, &header, sizeof(header));
        std::memcpy(buffer + sizeof(header), &payload, sizeof(Payload));

        int sent = sendto(serverSocket, reinterpret_cast<const char*>(buffer), sizeof(buffer), 0, reinterpret_cast<const sockaddr*>(&clientAddr), sizeof(clientAddr));

        if (sent == -1) std::cerr << "[Server] Failed to send packet to client\n";
    }

    Packet Receive();

    template<typename Payload>
    void Broadcast(const ServerPacketType type, const std::vector<Payload>& payloads) {
        for (auto& [playerID, addr] : clients) {
            for (auto& payload : payloads) Send(type, payload, addr);
        }
    }

    template<typename Payload>
    Payload ParsePayload(const Packet& packet) {
        if (packet.payload.size() < sizeof(Payload)) throw std::runtime_error("Pacote menor que o esperado");

        Payload payload;
        std::memcpy(&payload, packet.payload.data(), sizeof(Payload));
        return payload;
    }

    void RegisterClient(uint32_t playerID, const sockaddr_in& addr) {
        clients[playerID] = addr;
    }

private:
    #ifdef _WIN32
        using SocketType = uintptr_t;
    #else
        using SocketType = int;
    #endif
    
    SocketType serverSocket;
    sockaddr_in serverAddr;
    bool running;

    std::unordered_map<uint32_t, sockaddr_in> clients;

};