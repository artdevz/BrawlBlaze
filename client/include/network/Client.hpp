#pragma once

#include <cstring>
#include <cstdint>
#include <string>
#include <vector>
#include <stdexcept>

#include "../../../common/include/network/Packets.hpp"

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <sys/socket.h>
    #include <arpa/inet.h>
#endif
#include <iostream>
struct Packet {
    uint8_t type;
    std::vector<uint8_t> payload;
    size_t size;
    sockaddr_in addr;
};

class Client {

public:
    Client();
    ~Client();

    bool Connect(const std::string& host, uint16_t port);

    template<typename Payload>
    void Send(ClientPacketType type, const Payload& payload) {
        if (!connected) return;

        ClientPacketHeader header{};
        header.type = type;

        uint8_t buffer[sizeof(ClientPacketHeader) + sizeof(Payload)];
        std::memcpy(buffer, &header, sizeof(header));
        std::memcpy(buffer + sizeof(header), &payload, sizeof(Payload));

        int sent = sendto(clientSocket, reinterpret_cast<const char*>(buffer), sizeof(buffer), 0, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr));

        if (sent == -1) perror("[Client] Failed to send packet");
    }

    Packet Receive();

    template<typename Payload>
    Payload ParsePayload(const Packet& packet) {
        if (packet.payload.size() < sizeof(Payload)) throw std::runtime_error("Pacote menor que o esperado");

        Payload payload;
        std::memcpy(&payload, packet.payload.data(), sizeof(Payload));
        return payload;
    }

    void Stop();

private:
    #ifdef _WIN32
        using SocketType = uintptr_t;
    #else
        using SocketType = int;
    #endif
        SocketType clientSocket;
        sockaddr_in serverAddr;
        bool connected;
    
};