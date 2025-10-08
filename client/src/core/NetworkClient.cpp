#include "core/NetworkClient.hpp"

#include <iostream>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
#endif

Client::Client() : connected(false) {
    #ifdef _WIN32
        WSADATA wsa;
        WSAStartup(MAKEWORD(2, 2), &wsa);
    #endif
}


Client::~Client() { 
    Stop();
    #ifdef _WIN32
        WSACleanup();
    #endif
}

bool Client::Connect(const std::string& host, uint16_t port) {
    clientSocket =
    #ifdef _WIN32
        socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    #else
        socket(AF_INET, SOCK_DGRAM, 0);
    #endif

    if (clientSocket < 0) {
        std::cerr << "[Client] Failed to create socket\n";
        return false;
    }

    std::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    #ifdef _WIN32
        inet_pton(AF_INET, host.c_str(), &serverAddr.sin_addr);
    #else
        inet_aton(host.c_str(), &serverAddr.sin_addr);
    #endif

    if (::connect(clientSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) < 0) {
        perror("[Client] connect");
        return false;
    }

    connected = true;
    return true;
}

Packet Client::Receive() {
    uint8_t buffer[1024];
    socklen_t serverLen = sizeof(serverAddr);

    int bytesReceived = recvfrom(clientSocket, reinterpret_cast<char*>(buffer), sizeof(buffer), 0, (sockaddr*)&serverAddr, &serverLen);
    if (bytesReceived <= 0) {
        if (bytesReceived == -1) {
            // perror("[Client] recvfrom failed");
        }
        return {0, {}, 0, {}};
    }

    uint8_t packetType = buffer[0];
    // std::cout << "PacketType: " << (int)packetType << "\n";

    std::vector<uint8_t> payload;
    if (bytesReceived > sizeof(uint8_t)) {
        payload.resize(bytesReceived - sizeof(uint8_t));
        std::memcpy(payload.data(), buffer + sizeof(uint8_t), payload.size());
    }

    // std::cout << "Payload recebido: " << payload.size() << "\n";
    return {packetType, payload, static_cast<size_t>(bytesReceived), serverAddr};
}

void Client::Stop() {
    if (!connected) return;
    #ifdef _WIN32
        closesocket(clientSocket);
    #else
        close(clientSocket);
    #endif
        connected = false;
}