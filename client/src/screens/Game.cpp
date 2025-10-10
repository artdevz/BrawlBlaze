#include "screens/Game.hpp"

#include <iostream>
#include <string>
#include <unordered_set>

#include "raylib.h"

#include "core/AssetManager.hpp"
#include "core/CameraManager.hpp"
#include "core/InputManager.hpp"

#include "components/Sprite.hpp"

struct Direction { int8_t x, y = 0; };

void Game::Init() {
    shouldClose = false;
    TraceLog(LOG_INFO, "Starting the Game...");
    AssetManager::Get().LoadTextures();

    client = std::make_unique<Client>();
    if (!client->Connect("127.0.0.1", 8080)) {
        TraceLog(LOG_ERROR, "Failed to connect at Server");
        return;
    }
    networkManager.Start(client.get());

    ConnectPayload payload{};
    const char* nickname = "Artdz";
    std::strncpy(payload.nickname, nickname, sizeof(payload.nickname) - 1);
    payload.nickname[sizeof(payload.nickname) - 1] = '\0';
    client->Send(ClientPacketType::Connect, payload);

    while (true) {
        auto packet = client->Receive();
        if (packet.type == (uint8_t)ServerPacketType::Init) {
            std::cout << "Chegou!\n";
            auto initPayload = client->ParsePayload<InitPayload>(packet);
            Entity entity = entityManager.CreateEntity();
            entity.id = initPayload.entityID;
            entityManager.AddComponent(entity.id, Type(EntityType::Player));
            entityManager.AddComponent(entity.id, Position(initPayload.x, initPayload.y));
            entityManager.AddComponent(entity.id, Velocity(100.0f));
            entityManager.AddComponent(entity.id, Health(initPayload.hp, initPayload.maxHP));
            entityManager.AddComponent(entity.id, Collider(16.0f, 16.0f));
            entityManager.AddComponent(entity.id, Player());
            entityManager.AddComponent(entity.id, KDA());
            std::strncpy(entityManager.GetComponent<Player>(entity.id).nickname, initPayload.nickname, sizeof(initPayload.nickname) - 1);
            entityManager.GetComponent<Player>(entity.id).nickname[sizeof(initPayload.nickname) - 1] = '\0';
            entityManager.AddComponent(entity.id, Team(initPayload.team > 0 ? (TeamColor)initPayload.team : TeamColor::None));
            entityManager.AddComponent(entity.id, Sprite("human"));
            localPlayerID = initPayload.entityID;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

Direction direction;
static std::unordered_set<int> spawnedEntities;

void Game::Update() {
    float deltaTime = GetFrameTime();

    // ===== Prediction ===== //

    direction = {0, 0};
    if (InputManager::IsMoveUpPressed()) direction.y--;
    if (InputManager::IsMoveDownPressed()) direction.y++;
    if (InputManager::IsMoveLeftPressed()) direction.x--;
    if (InputManager::IsMoveRightPressed()) direction.x++;

    float speed = 100.0f;
    if (auto* velocity = entityManager.TryGetComponent<Velocity>(localPlayerID)) {
        velocity->dx = static_cast<float>(direction.x) * speed;
        velocity->dy = static_cast<float>(direction.y) * speed;
    }
    movement.Move(entityManager, deltaTime);
    InputPayload payload{};
    payload.playerID = localPlayerID;
    // payload.inputSequence ++inputSequence;
    payload.x = direction.x;
    payload.y = direction.y;
    if (InputManager::IsPickUpFlagPressed()) payload.isPickUsed = true;
    if (InputManager::IsBasicAttackPressed()) {
        Vector2 worldPosition = GetScreenToWorld2D(GetMousePosition(), CameraManager::Get().GetCamera2D());
        payload.targetX = worldPosition.x;
        payload.targetY = worldPosition.y;
        payload.isMouseUsed = true;
    }
    payload.deltaTime = deltaTime;

    client->Send(ClientPacketType::Input, payload);

    // ===== Spawn ===== //

    AddEntityPayload addPayload{};
    while (networkManager.PoolPacket(networkManager.addQueue, networkManager.addMutex, addPayload)) {
        if (addPayload.entityID == localPlayerID) continue;
        if (spawnedEntities.count(addPayload.entityID)) continue;
        spawnedEntities.insert(addPayload.entityID);

        Entity entity = entityManager.CreateEntity();
        entity.id = addPayload.entityID;
        entityManager.AddComponent(entity.id, Type((EntityType)addPayload.type));
        std::cout << "Spawning Entity ID: " << entity.id << " Type: " << (int)addPayload.type << "\n";
        entityManager.AddComponent(entity.id, Position(addPayload.x, addPayload.y));

        if (addPayload.type == (uint16_t)EntityType::Player) {
            entityManager.AddComponent(entity.id, Collider(16.0f, 16.0f));
            entityManager.AddComponent(entity.id, Player());
            entityManager.AddComponent(entity.id, KDA());
            if (addPayload.nickname[0] != '\0') {
                std::strncpy(entityManager.GetComponent<Player>(entity.id).nickname, addPayload.nickname, sizeof(addPayload.nickname) - 1);
                entityManager.GetComponent<Player>(entity.id).nickname[sizeof(addPayload.nickname) - 1] = '\0';
            }
            entityManager.AddComponent(entity.id, Sprite("human"));
        }

        if (addPayload.type == (uint16_t)EntityType::FloorTile) {
            entityManager.AddComponent(entity.id, Type(EntityType::FloorTile));
            entityManager.AddComponent(entity.id, TileTag());
            entityManager.AddComponent(entity.id, Position(addPayload.x, addPayload.y));
            entityManager.AddComponent(entity.id, Sprite("floor"));
        }

        if (addPayload.type == (uint16_t)EntityType::Projectile) {
            entityManager.AddComponent(entity.id, Type(EntityType::Projectile));
            entityManager.AddComponent(entity.id, Sprite("bullet"));
            entityManager.AddComponent(entity.id, Projectile(localPlayerID));
        }

        if (addPayload.type == (uint16_t)EntityType::Tower) {
            entityManager.AddComponent(entity.id, Type(EntityType::Tower));
            entityManager.AddComponent(entity.id, Collider(16.0f, 16.0f));
            entityManager.AddComponent(entity.id, Sprite("tower"));
        }
        if (addPayload.type == (uint16_t)EntityType::Flag) {
            entityManager.AddComponent(entity.id, Type(EntityType::Flag));
            entityManager.AddComponent(entity.id, Sprite("flag"));
        }

        entityManager.AddComponent(entity.id, Health(addPayload.hp, addPayload.maxHP));
        if (addPayload.team > 0) {
            std::cout << "Entity ID: " << entity.id << " Team: " << (int)addPayload.team << "\n";
            entityManager.AddComponent(entity.id, Team((TeamColor)addPayload.team));
        }
    }

    // ===== Interpolation ===== //

    EntityStatePayload statePayload{};
    while (networkManager.PoolPacket(networkManager.stateQueue, networkManager.stateMutex, statePayload)) {
        if (auto* position = entityManager.TryGetComponent<Position>(statePayload.entityID)) {
            // if (statePayload.entityID == localPlayerID) continue; // Temporário
            position->x = statePayload.x;
            position->y = statePayload.y;
        }
        if (auto* health = entityManager.TryGetComponent<Health>(statePayload.entityID)) health->current = statePayload.hp;
    }

    // ===== Despawn ===== //

    RemoveEntityPayload removePayload{};
    while (networkManager.PoolPacket(networkManager.removeQueue, networkManager.removeMutex, removePayload)) {
        if (removePayload.entityID) {
            std::cout << "Removing Entity ID: " << removePayload.entityID << "\n";
            entityManager.DeleteEntity(removePayload.entityID);
            spawnedEntities.erase(removePayload.entityID);
        }
    }

    // ===== Combat Stats ===== //

    CombatStatsPaylod combatStatsPayload{};
    while (networkManager.PoolPacket(networkManager.combatStatsQueue, networkManager.combatStatsMutex, combatStatsPayload)) {
        if (auto* kda = entityManager.TryGetComponent<KDA>(combatStatsPayload.entityID)) {
            kda->kills = combatStatsPayload.kills;
            kda->deaths = combatStatsPayload.deaths;
            kda->assists = combatStatsPayload.assists;
        }
    }

    // ===== Match Stats ===== //

    MatchStatsPayload matchStatsPayload{};
    while (networkManager.PoolPacket(networkManager.matchStatsQueue, networkManager.matchStatsMutex, matchStatsPayload)) {
        matchTime.time = matchStatsPayload.time;
    }

    // ===== Team Change ===== //

    EntityTeamChangePayload teamChangePayload{};
    while (networkManager.PoolPacket(networkManager.teamChangeQueue, networkManager.teamChangeMutex, teamChangePayload)) {
        if (auto* team = entityManager.TryGetComponent<Team>(teamChangePayload.entityID)) {
            team->color = (TeamColor)teamChangePayload.newTeam;
        }
    }

    // ===== Camera ===== //
    
    if (auto* position = entityManager.TryGetComponent<Position>(localPlayerID)) CameraManager::Get().Update({position->x, position->y});
}

void Game::Draw() {
    BeginMode2D(CameraManager::Get().GetCamera2D());
    ClearBackground(BLACK);
    DrawRectangle(-256, 128, 128, 128, BLUE);
    DrawRectangle(128, -256, 128, 128, RED);
    render.RenderTile(entityManager);
    render.RenderActor(entityManager, localPlayerID);
    render.RenderLifebar(entityManager, localPlayerID);
    EndMode2D();
    if (auto* hp = entityManager.TryGetComponent<Health>(localPlayerID)) {
        if (hp->IsDead()) {
            DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(GRAY, 0.5f));
            DrawText("You are dead!", GetScreenWidth()/2 - MeasureText("You are dead!", 40)/2, GetScreenHeight()/2 - 20, 40, RED);
        }
    }

    std::string entitiesCount = "E: " + std::to_string((int)entityManager.GetEntities().size());
    std::string position = "X: " + std::to_string((float)CameraManager::Get().GetCamera2D().target.x) + " Y: " + std::to_string((float)CameraManager::Get().GetCamera2D().target.y) + " Zoom: " + std::to_string((float)CameraManager::Get().GetCamera2D().zoom);
    DrawText(entitiesCount.c_str(), 10, 30, 20, WHITE);
    DrawText(position.c_str(), 10, 50, 20, WHITE);
    if (auto* kda = entityManager.TryGetComponent<KDA>(localPlayerID)) {
        std::string kdaText = "KDA: " + std::to_string(kda->kills) + " / " + std::to_string(kda->deaths) + " / " + std::to_string(kda->assists);
        DrawText(kdaText.c_str(), 10, 70, 20, WHITE);
    }
    std::string timeText = ((matchTime.GetMinutes() < 10)? "0" + std::to_string(matchTime.GetMinutes()) : std::to_string(matchTime.GetMinutes())) + ":" + ((matchTime.GetSeconds() < 10)? "0" + std::to_string(matchTime.GetSeconds()) : std::to_string(matchTime.GetSeconds()));
    DrawText(timeText.c_str(), 10, 90, 20, WHITE);

    DrawFPS(10, 10);
}

bool Game::ShouldClose() const { return shouldClose; }

ScreenType Game::NextScreen() {
    return ScreenType::None;
}