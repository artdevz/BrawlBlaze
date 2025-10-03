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
            case ServerPacketType::EntityState: {
                auto payload = client->ParsePayload<EntityStatePayload>(result);
                std::lock_guard<std::mutex> lock(stateMutex);
                stateQueue.push(payload);
                break;
            }
            case ServerPacketType::Remove: {
                auto payload = client->ParsePayload<RemoveEntityPayload>(result);
                std::lock_guard<std::mutex> lock(removeMutex);
                removeQueue.push(payload);
                break;
            }
            case ServerPacketType::CombatStats: {
                auto payload = client->ParsePayload<CombatStatsPaylod>(result);
                std::lock_guard<std::mutex> lock(combatStatsMutex);
                combatStatsQueue.push(payload);
                break;
            }  
            default:
                break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }    
}