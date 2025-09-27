#include "core/NetworkManager.hpp"


#include <iostream>
#include <cstring>

NetworkManager::NetworkManager() : running(false) {}

NetworkManager::~NetworkManager() { Stop(); }

void NetworkManager::Start(Client* client) {
    running = true;
    worker = std::thread(&NetworkManager::NetworkThread, this, client);
    worker.detach();
}

void NetworkManager::Stop() {
    running = false;
}

void NetworkManager::NetworkThread(Client* client) {
    while (running) {
        auto result = client->Receive();
        switch ((ServerPacketType)result.type) {
            case ServerPacketType::Add: {
                auto payload = client->ParsePayload<AddEntityPayload>(result);
                std::lock_guard<std::mutex> lock(addMutex);
                addQueue.push(payload);
                break;
            }            
            default:
                break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }    
}