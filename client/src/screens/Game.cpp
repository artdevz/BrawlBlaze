#include "screens/Game.hpp"

#include <iostream>
#include <string>

#include "raylib.h"

Game::Game() {}

void Game::Init() {
    shouldClose = false;
    TraceLog(LOG_INFO, "Starting the Game...");

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
    std::cout << "GGGGGGGGGGGGGGGG\n";
    client->Send(ClientPacketType::Connect, payload);
}
void Game::Update() {
    std::cout << "Game\n";
}
void Game::Render() {
    DrawText("Game", 16, 16, 16, WHITE);
}

bool Game::ShouldClose() const { return shouldClose; }

ScreenType Game::NextScreen() {
    return ScreenType::None;
}