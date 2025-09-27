#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <unordered_map>

#include "network/Server.hpp"

#include "../../common/include/EntityManager.hpp"

#include "../../common/include/systems/Movement.hpp"

using std::string, std::cout;

int main(int argc, char** argv) {
    cout << "[Server] Server starting...\n";

    Server server;
    if (!server.Start(8080)) return 1;

    EntityManager entityManager;
    Movement movement;

    bool running = true;

    std::queue<InputPayload> inputQueue;
    std::mutex inputMutex;

    std::mutex entityMutex;
    std::mutex serverMutex;

    std::thread inputThread([&]() {
        while (running) {
            auto packet = server.Receive();
            if (packet.type == 0) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                continue;
            }

            try {
                switch (static_cast<ClientPacketType>(packet.type)) {
                    case ClientPacketType::Connect: {
                        auto payload = server.ParsePayload<ConnectPayload>(packet);
                        Entity newPlayer;
                        {
                            newPlayer = entityManager.CreateEntity();
                            entityManager.AddComponent(newPlayer.id, Type(EntityType::Player));
                            entityManager.AddComponent(newPlayer.id, Position(0.0f, 0.0f));
                            entityManager.AddComponent(newPlayer.id, Velocity());
                        }

                        {
                            server.RegisterClient(newPlayer.id, packet.addr);
                        }
                        cout << "[Server] " << payload.nickname << " joined the game\n";

                        InitPayload initPayload{};
                        initPayload.entityID = newPlayer.id;
                        if (auto* position = entityManager.TryGetComponent<Position>(newPlayer.id)) {
                            initPayload.x = position->x;
                            initPayload.y = position->y;
                        }
                        for (int i = 0; i < 1000; i++) server.Send(ServerPacketType::Init, initPayload, packet.addr);
                    }

                    case ClientPacketType::Input: {
                        auto payload = server.ParsePayload<InputPayload>(packet);
                        std::lock_guard<std::mutex> lock(inputMutex);
                        inputQueue.push(payload);
                        break;
                    }
                }
            }
            catch (const std::exception& e) { std::cerr << "[Server] Failed to parse packet: " << e.what() << "\n"; }

            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    });

    std::thread simulateThread([&]() {
        while (running) {
            {
                std::lock_guard<std::mutex> lockInput(inputMutex);
                std::lock_guard<std::mutex> lockEntity(entityMutex);
                while (!inputQueue.empty()) {
                    auto input = inputQueue.front();
                    inputQueue.pop();

                    if (auto* velocity = entityManager.TryGetComponent<Velocity>(input.playerID)) {
                        float speed = 100.0f;
                        if (input.x > 0) {input.x = 1.0f;} if (input.x < 0) {input.x = -1.0f;} if (input.y > 0) {input.y = 1.0f;} if (input.y < 0) {input.y = -1.0f;}
                        velocity->dx = input.x * speed; velocity->dy = input.y * speed;
                    }
                }
            }
            movement.Move(entityManager, 1.0f / 60.0f);

            for (auto entity : entityManager.GetEntities<Position>()) {
                auto position = entityManager.TryGetComponent<Position>(entity.id);
                cout << "Entity[" << entity.id << "]: x: " << position->x << " y: " << position->y << "\n";
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
    });

    std::thread broadcastThread([&]() {
        while (running) {
            std::vector<EntityStatePayload> stateSnapshot;

            // ===== Generate ===== //

            {
                std::lock_guard<std::mutex> lock(entityMutex);

                for (auto& entity : entityManager.GetEntities<Position, Velocity>()) {
                    auto& position = entityManager.GetComponent<Position>(entity.id);
                    EntityStatePayload payload{};
                    payload.entityID = entity.id;
                    payload.x = position.x;
                    payload.y = position.y;
                    stateSnapshot.push_back(payload);
                }
            }

            // ===== Send ===== //

            if (!stateSnapshot.empty()) {
                std::lock_guard<std::mutex> lock(serverMutex);
                server.Broadcast<EntityStatePayload>(ServerPacketType::EntityState, stateSnapshot);
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    });

    cout << "[Server] Press Enter to STOP\n";
    std::cin.get();
    running = false;

    inputThread.join();

    server.Stop();
    cout << "[Server] Shutdown.\n";
}