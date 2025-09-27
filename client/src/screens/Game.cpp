#include "screens/Game.hpp"

#include <iostream>
#include <string>

#include "raylib.h"

#include "core/AssetManager.hpp"
#include "core/CameraManager.hpp"

#include "components/Sprite.hpp"

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
            entityManager.AddComponent(entity.id, Sprite("human"));
            localPlayerID = initPayload.entityID;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void Game::Update() {
    if (auto* position = entityManager.TryGetComponent<Position>(localPlayerID)) CameraManager::Get().Update({position->x, position->y});
}

void Game::Draw() {
    BeginMode2D(CameraManager::Get().GetCamera2D());
    ClearBackground(BLACK);
    render.RenderTile(entityManager);
    render.RenderActor(entityManager);
    EndMode2D();
}

bool Game::ShouldClose() const { return shouldClose; }

ScreenType Game::NextScreen() {
    return ScreenType::None;
}