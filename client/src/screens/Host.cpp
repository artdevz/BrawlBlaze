#include "screens/Host.hpp"

#include "raylib.h"

Host::Host() {}

void Host::Init() {
    showGameScreen = true;
}
void Host::Update() {}
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