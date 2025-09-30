#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <unordered_map>

#include "network/Server.hpp"

#include "../../common/include/EntityManager.hpp"

#include "systems/Combat.hpp"
#include "../../common/include/systems/Movement.hpp"

#include "components/Lifetime.hpp"

using std::string, std::cout;

int main(int argc, char** argv) {
    cout << "[Server] Server starting...\n";

    Server server;
    if (!server.Start(8080)) return 1;

    EntityManager entityManager;
    Combat combat;
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
                            entityManager.AddComponent(newPlayer.id, Velocity(100.0f, 100.0f));
                            entityManager.AddComponent(newPlayer.id, Collider(16.0f, 16.0f));
                            entityManager.AddComponent(newPlayer.id, Health(100.0f, 100.0f));
                            entityManager.AddComponent(newPlayer.id, Team(TeamColor::Blue));
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
    bool initialized = false; // Temporário
    std::thread simulateThread([&]() {
        using clock = std::chrono::high_resolution_clock;
        auto lastTime = clock::now();

        while (running) {
            auto now = clock::now();
            std::chrono::duration<float> elapsed = now - lastTime;
            float deltaTime = elapsed.count();
            if (deltaTime < 0.0f) deltaTime = 0.001f;
            lastTime = now;

            // ===== Generate ===== //

            if (!initialized) {
                Entity ally = entityManager.CreateEntity();
                entityManager.AddComponent(ally.id, Type(EntityType::Player));
                entityManager.AddComponent(ally.id, Position(-160.0f, 0.0f));
                entityManager.AddComponent(ally.id, Velocity(100.0f));
                entityManager.AddComponent(ally.id, Collider(16.0f, 16.0f));
                entityManager.AddComponent(ally.id, Health(100.0f, 100.0f));
                entityManager.AddComponent(ally.id, Team(TeamColor::Blue));
                
                Entity dummy = entityManager.CreateEntity();
                entityManager.AddComponent(dummy.id, Type(EntityType::Player));
                entityManager.AddComponent(dummy.id, Position(160.0f, 0.0f));
                entityManager.AddComponent(dummy.id, Velocity(100.0f));
                entityManager.AddComponent(dummy.id, Collider(16.0f, 16.0f));
                entityManager.AddComponent(dummy.id, Health(100.0f, 100.0f));
                entityManager.AddComponent(dummy.id, Team(TeamColor::Red));
                initialized = true;
            }

            // ===== Process Inputs ===== //

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

                    if (input.isMouseUsed) {
                        // cout << "Mouse usado!\n";
                        // cout << "TargetX: " << input.targetX << " TargetY: " << input.targetY << "\n";
                        if (entityManager.GetEntities().size() > 3) continue; // Temporário
                        Entity projectile = entityManager.CreateEntity();
                        entityManager.AddComponent(projectile.id, Type(EntityType::Projectile));
                        if (auto* origin = entityManager.TryGetComponent<Position>(input.playerID)) {
                            entityManager.AddComponent(projectile.id, Position(origin->x, origin->y));
                            entityManager.AddComponent(projectile.id, Velocity(600.0f, (input.targetX - origin->x), (input.targetY - origin->y)));
                            // entityManager.AddComponent(projectile.id, Collider(4.0f, 4.0f));
                            entityManager.AddComponent(projectile.id, Lifetime(100.0f));
                            entityManager.AddComponent(projectile.id, Projectile(input.playerID));
                            entityManager.AddComponent(projectile.id, Team(entityManager.GetComponent<Team>(input.playerID).color));
                        }
                    }
                }
            }
            combat.HandleProjectiles(entityManager);
            movement.Move(entityManager, deltaTime);

            for (auto entity : entityManager.GetEntities<Lifetime>()) {
                auto& lifetime = entityManager.GetComponent<Lifetime>(entity.id);
                lifetime.lifespan -= 1.0f;
                //cout << "Entity[" << entity.id << "] Lifetime: " << lifetime.lifespan << "\n";
                if (lifetime.lifespan <= 0.0f) {
                    // cout << "[Server] Entity ID: " << entity.id << " expired.\n";
                    entityManager.AddComponent(entity.id, RemoveTag());
                }
            }

            for (auto entity : entityManager.GetEntities<Position>()) {
                auto position = entityManager.TryGetComponent<Position>(entity.id);
                // cout << "Entity[" << entity.id << "]: x: " << position->x << " y: " << position->y << "\n";
            }
            // cout << "EntityManagerSize: " << (int)entityManager.GetEntities().size() << "\n";

            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
    });

    std::thread broadcastThread([&]() {
        while (running) {
            std::vector<EntityStatePayload> stateSnapshot;
            std::vector<AddEntityPayload> addSnapshot;
            std::vector<RemoveEntityPayload> removeSnapshot;

            // ===== Generate ===== //

            {
                std::lock_guard<std::mutex> lock(entityMutex);

                // ===== Entity States ===== //

                for (auto& entity : entityManager.GetEntities<Position, Velocity>()) {
                    EntityStatePayload payload{};
                    payload.entityID = entity.id;
                    auto& position = entityManager.GetComponent<Position>(entity.id);
                    payload.x = position.x;
                    payload.y = position.y;
                    if (auto* health = entityManager.TryGetComponent<Health>(entity.id)) payload.hp = health->current;
                    stateSnapshot.push_back(payload);
                }

                // ===== Add ===== //

                for (auto& entity : entityManager.GetEntities<Position>()) {
                    AddEntityPayload payload{};
                    payload.entityID = entity.id;
                    if (auto* type = entityManager.TryGetComponent<Type>(entity.id)) {
                        payload.type = (uint16_t)type->type;
                    }                    
                    auto& position = entityManager.GetComponent<Position>(entity.id);
                    payload.x = position.x;
                    payload.y = position.y;
                    if (auto* health = entityManager.TryGetComponent<Health>(entity.id)) {
                        payload.hp = health->current;
                        payload.maxHP = health->max;
                    }
                    if (auto* team = entityManager.TryGetComponent<Team>(entity.id)) {
                        payload.team = (uint8_t)team->color;
                    }
                    addSnapshot.push_back(payload);
                }

                // ===== Remove ===== //

                for (auto& entity : entityManager.GetEntities<RemoveTag>()) {
                    RemoveEntityPayload payload{};
                    payload.entityID = entity.id;
                    removeSnapshot.push_back(payload);
                    entityManager.DeleteEntity(entity.id);
                    // cout << "[Server] Entity ID: " << entity.id << " removed.\n";
                }
            }

            // ===== Send ===== //

            if (!stateSnapshot.empty()) {
                std::lock_guard<std::mutex> lock(serverMutex);
                server.Broadcast<EntityStatePayload>(ServerPacketType::EntityState, stateSnapshot);
            }

            if (!addSnapshot.empty()) {
                std::lock_guard<std::mutex> lock(serverMutex);
                server.Broadcast<AddEntityPayload>(ServerPacketType::Add, addSnapshot);
            }

            if (!removeSnapshot.empty()) {
                std::lock_guard<std::mutex> lock(serverMutex);
                server.Broadcast<RemoveEntityPayload>(ServerPacketType::Remove, removeSnapshot);
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    });

    cout << "[Server] Press Enter to STOP\n";
    std::cin.get();
    running = false;

    inputThread.join();
    simulateThread.join();
    broadcastThread.join();

    server.Stop();
    cout << "[Server] Shutdown.\n";
}