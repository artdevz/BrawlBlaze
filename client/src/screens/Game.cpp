#include "screens/Game.hpp"

#include <iostream>
#include <string>

#include "raylib.h"

#include "core/AssetManager.hpp"
#include "core/CameraManager.hpp"
#include "core/InputManager.hpp"

#include "components/Sprite.hpp"

struct Direction { int8_t x, y = 0; };

Game::Game() {}

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
            entityManager.AddComponent(entity.id, Velocity(0.0f, 0.0f));
            entityManager.AddComponent(entity.id, Sprite("human"));
            localPlayerID = initPayload.entityID;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

Direction direction;
void Game::Update() {
    float deltaTime = GetFrameTime();

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
    if (auto* position = entityManager.TryGetComponent<Position>(localPlayerID)) CameraManager::Get().Update({position->x, position->y});
}

void Game::Draw() {
    BeginMode2D(CameraManager::Get().GetCamera2D());
    ClearBackground(BLACK);
    render.RenderTile(entityManager);
    render.RenderActor(entityManager);
    DrawRectangle(160, 160, 16, 16, GRAY);
    EndMode2D();

    std::string entitiesCount = "E: " + std::to_string((int)entityManager.GetEntities().size());
    std::string position = "X: " + std::to_string((float)CameraManager::Get().GetCamera2D().target.x) + " Y: " + std::to_string((float)CameraManager::Get().GetCamera2D().target.y) + " Zoom: " + std::to_string((float)CameraManager::Get().GetCamera2D().zoom);
    DrawText(entitiesCount.c_str(), 10, 700, 20, WHITE);
    DrawText(position.c_str(), 10, 680, 20, WHITE);

    DrawFPS(10, 10);
}

bool Game::ShouldClose() const { return shouldClose; }

ScreenType Game::NextScreen() {
    return ScreenType::None;
}