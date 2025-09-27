#include "screens/Host.hpp"

#include "raylib.h"

Host::Host() {}

void Host::Init() {
    showGameScreen = false;
}
void Host::Update() {
    if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE)) shouldClose = true;
    if (IsKeyPressed(KEY_ENTER)) showGameScreen = true;
}

void Host::Draw() {
    DrawText("Host", 16, 16, 16, WHITE);
}

bool Host::ShouldClose() const { return shouldClose; }

ScreenType Host::NextScreen() {
    if (showGameScreen) {
        showGameScreen = false;
        return ScreenType::Game;
    }
    return ScreenType::None;
}