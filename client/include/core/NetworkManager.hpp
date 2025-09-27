#pragma once

#include <atomic>
#include <mutex>
#include <queue>
#include <thread>

#include "network/Client.hpp"

class NetworkManager {

public:
    NetworkManager();
    ~NetworkManager();

    std::queue<AddEntityPayload> addQueue;
    std::mutex addMutex;

    std::queue<EntityStatePayload> stateQueue;
    std::mutex stateMutex;

    void Start(Client* client);
    void Stop();

    template<typename T>
    bool PoolPacket(std::queue<T>& queue, std::mutex& mtx, T& outPacket) {
        std::lock_guard<std::mutex> lock(mtx);
        if (queue.empty()) return false;
        outPacket = queue.front();
        queue.pop();
        return true;
    }

private:
    std::atomic<bool> running;
    std::thread worker;

    void NetworkThread(Client* client);

};