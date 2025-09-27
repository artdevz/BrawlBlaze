#include "network/Server.hpp"

#include <iostream>
#include <cstring>

Server::Server() : running(false) {
    #ifdef _WIN32
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);
    #endif
}

Server::~Server() {
    Stop();
    #ifdef _WIN32
        WSACleanup();
    #endif
}

bool Server::Start(uint16_t port) {
    serverSocket = 
    #ifdef _WIN32
        socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    #else
        socket(AF_INET, SOCK_DGRAM, 0);
    #endif

    if (serverSocket < 0) {
        std::cerr << "[Server] Failed to create socket\n";
        return false;
    }

    std::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "[Server] Failed to bind socket\n";
        return false;
    }

    std::cout << "[Server] Listening on port " << port << "\n";
    running = true;
    return true;
}

void Server::Stop() {
    if (!running) return;
    #ifdef _WIN32
        closesocket(serverSocket);
    #else
        close(serverSocket);
    #endif

    running = false;
}

Packet Server::Receive() {
    if (!running) return {0, {}, 0, {}};

    uint8_t buffer[1024];
    sockaddr_in clientAddr;
    socklen_t clientLen = sizeof(clientAddr);

    int bytesReceived = recvfrom(serverSocket, reinterpret_cast<char*>(buffer), sizeof(buffer), MSG_DONTWAIT, (sockaddr*)&clientAddr, &clientLen);
    if (bytesReceived <= 0) return {0, {}, 0, {}};

    uint8_t packetType = buffer[0];

    std::vector<uint8_t> payload;
    if (bytesReceived > sizeof(uint8_t)) {
        payload.resize(bytesReceived - sizeof(uint8_t));
        std::memcpy(payload.data(), buffer + sizeof(uint8_t), payload.size());
    }

    return {packetType, payload, static_cast<size_t>(bytesReceived), clientAddr};
}