#pragma once

#include <cstdint>
#include <unordered_map>
#include <vector>

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

    void Send();
    Packet Receive();
    void Broadcast();

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