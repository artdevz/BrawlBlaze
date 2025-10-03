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
    uint8_t playersConnected = 0;

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
                            entityManager.AddComponent(newPlayer.id, Player());
                            entityManager.AddComponent(newPlayer.id, KDA());
                            std::strncpy(entityManager.GetComponent<Player>(newPlayer.id).nickname, payload.nickname, sizeof(payload.nickname) - 1);
                            entityManager.GetComponent<Player>(newPlayer.id).nickname[sizeof(payload.nickname) - 1] = '\0';
                            // Alterna entre times
                            entityManager.AddComponent(newPlayer.id, Team( (playersConnected % 2? TeamColor::Red : TeamColor::Blue) ));
                        }

                        {
                            server.RegisterClient(newPlayer.id, packet.addr);
                        }
                        cout << "[Server] " << payload.nickname << " joined the game\n";
                        cout << "[Server] " << (int)++playersConnected << "/255 players connected\n";

                        InitPayload initPayload{};
                        initPayload.entityID = newPlayer.id;
                        if (auto* position = entityManager.TryGetComponent<Position>(newPlayer.id)) {
                            initPayload.x = position->x;
                            initPayload.y = position->y;
                        }
                        if (auto* health = entityManager.TryGetComponent<Health>(newPlayer.id)) {
                            initPayload.hp = health->current;
                            initPayload.maxHP = health->max;
                        }
                        initPayload.team = (uint8_t)entityManager.GetComponent<Team>(newPlayer.id).color;
                        std::strncpy(initPayload.nickname, payload.nickname, sizeof(initPayload.nickname) - 1);
                        initPayload.nickname[sizeof(initPayload.nickname) - 1] = '\0';
                        for (int i = 0; i < 1000; i++) server.Send(ServerPacketType::Init, initPayload, packet.addr);
                    }

                    case ClientPacketType::Input: {
                        auto payload = server.ParsePayload<InputPayload>(packet);
                        std::lock_guard<std::mutex> lock(inputMutex);
                        inputQueue.push(payload);
                        break;
                    }
                    case ClientPacketType::Message: {
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
                entityManager.AddComponent(ally.id, Player());
                entityManager.AddComponent(ally.id, KDA());
                std::strncpy(entityManager.GetComponent<Player>(ally.id).nickname, "BlueDummy", sizeof("BlueDummy") - 1);
                entityManager.GetComponent<Player>(ally.id).nickname[sizeof("BlueDummy") - 1] = '\0';
                entityManager.AddComponent(ally.id, Team(TeamColor::Blue));
                
                Entity dummy = entityManager.CreateEntity();
                entityManager.AddComponent(dummy.id, Type(EntityType::Player));
                entityManager.AddComponent(dummy.id, Position(160.0f, 0.0f));
                entityManager.AddComponent(dummy.id, Velocity(100.0f));
                entityManager.AddComponent(dummy.id, Collider(16.0f, 16.0f));
                entityManager.AddComponent(dummy.id, Health(100.0f, 100.0f));
                entityManager.AddComponent(dummy.id, Player());
                entityManager.AddComponent(dummy.id, KDA());
                std::strncpy(entityManager.GetComponent<Player>(dummy.id).nickname, "RedDummy", sizeof("RedDummy") - 1);
                entityManager.GetComponent<Player>(dummy.id).nickname[sizeof("RedDummy") - 1] = '\0';
                entityManager.AddComponent(dummy.id, Team(TeamColor::Red));

                for (float i = 0.0f; i < 64.0f; i += 16.0f) for (float j = 0.0f; j < 64.0f; j += 16.0f) {
                    Entity floor = entityManager.CreateEntity();
                    entityManager.AddComponent(floor.id, Type(EntityType::FloorTile));
                    entityManager.AddComponent(floor.id, Position(i, j));
                }

                initialized = true;
            }

            // ===== Process Inputs ===== //

            {
                std::lock_guard<std::mutex> lockInput(inputMutex);
                std::lock_guard<std::mutex> lockEntity(entityMutex);
                while (!inputQueue.empty()) {
                    auto input = inputQueue.front();
                    inputQueue.pop();

                    auto* hp = entityManager.TryGetComponent<Health>(input.playerID);
                    if (!hp) continue;
                    if (hp->current <= 0.0f) continue;

                    if (auto* velocity = entityManager.TryGetComponent<Velocity>(input.playerID)) {
                        float speed = 100.0f;
                        if (input.x > 0) {input.x = 1.0f;} if (input.x < 0) {input.x = -1.0f;} if (input.y > 0) {input.y = 1.0f;} if (input.y < 0) {input.y = -1.0f;}
                        velocity->dx = input.x * speed; velocity->dy = input.y * speed;
                    }

                    if (input.isMouseUsed) {
                        // cout << "Mouse usado!\n";
                        // cout << "TargetX: " << input.targetX << " TargetY: " << input.targetY << "\n";
                        if (entityManager.GetEntities().size() > 20) continue; // Temporário
                        cout << "[Server] Player ID: " << input.playerID << " fired a projectile!\n";
                        Entity projectile = entityManager.CreateEntity();
                        cout << "[Server] Projectile ID: " << projectile.id << "\n";
                        entityManager.AddComponent(projectile.id, Type(EntityType::Projectile));
                        if (auto* origin = entityManager.TryGetComponent<Position>(input.playerID)) {
                            entityManager.AddComponent(projectile.id, Position(origin->x, origin->y));
                            entityManager.AddComponent(projectile.id, Velocity(600.0f, (input.targetX - origin->x), (input.targetY - origin->y)));
                            entityManager.AddComponent(projectile.id, Lifetime(100.0f));
                            entityManager.AddComponent(projectile.id, Projectile(input.playerID));
                            entityManager.AddComponent(projectile.id, Team(entityManager.GetComponent<Team>(input.playerID).color));
                        }
                        // cout << "[Server] Projectile spawned at (" << entityManager.GetComponent<Position>(projectile.id).x << ", " << entityManager.GetComponent<Position>(projectile.id).y << ")\n";
                    }
                }
            }
            movement.Move(entityManager, deltaTime);
            combat.HandleProjectiles(entityManager);

            for (auto entity : entityManager.GetEntities<Dead>()) {
                auto& dead = entityManager.GetComponent<Dead>(entity.id);
                dead.respawnTime--;
                if (dead.respawnTime <= 0) {
                    if (auto* player = entityManager.TryGetComponent<Player>(entity.id)) {
                        if (auto* position = entityManager.TryGetComponent<Position>(entity.id)) {
                            position->x = (entityManager.GetComponent<Team>(entity.id).color == TeamColor::Blue ? -128.0f : 128.0f);
                            position->y = 0.0f;
                        }
                        if (auto* health = entityManager.TryGetComponent<Health>(entity.id)) {
                            health->current = health->max;
                        }
                        // entityManager.GetComponent<Dead>(entity.id).respawnTime = 3000;
                        // entityManager.TryGetComponent<Dead>(entity.id);
                        entityManager.RemoveComponent<Dead>(entity.id);
                        cout << "[Server] Player " << player->nickname << " has respawned!\n";
                    }
                }
            }

            for (auto entity : entityManager.GetEntities<Lifetime>()) {
                if (entityManager.TryGetComponent<RemoveTag>(entity.id)) continue;
                auto& lifetime = entityManager.GetComponent<Lifetime>(entity.id);
                lifetime.lifespan -= 1.0f;
                //cout << "Entity[" << entity.id << "] Lifetime: " << lifetime.lifespan << "\n";
                if (lifetime.lifespan <= 0.0f) {
                    // cout << "[Server] Entity ID: " << entity.id << " expired.\n";
                    entityManager.AddComponent(entity.id, RemoveTag());
                }
            }

            for (auto entity : entityManager.GetEntities<KDA>()) {
                auto& kda = entityManager.GetComponent<KDA>(entity.id);
                cout << "Entity[" << entity.id << "] K: " << kda.kills << " D: " << kda.deaths << " A: " << kda.assists << "\n";
            }

            /*
            for (auto entity : entityManager.GetEntities<Position>()) {
                auto position = entityManager.TryGetComponent<Position>(entity.id);
                cout << "Entity[" << entity.id << "]: x: " << position->x << " y: " << position->y << "\n";
            }*/
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
                    if (auto* player = entityManager.TryGetComponent<Player>(entity.id)) {
                        std::strncpy(payload.nickname, player->nickname, sizeof(payload.nickname) - 1);
                        payload.nickname[sizeof(payload.nickname) - 1] = '\0';
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