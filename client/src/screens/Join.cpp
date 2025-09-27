#include "screens/Join.hpp"

#include "raylib.h"

Join::Join() {}

void Join::Init() {
    showGameScreen = true;
}
void Join::Update() {}
void Join::Render() {
    DrawText("Join", 16, 16, 16, WHITE);
}

bool Join::ShouldClose() const { return shouldClose; }

ScreenType Join::NextScreen() {
    if (showGameScreen) {
        showGameScreen = false;
        return ScreenType::Game;
    }
    return ScreenType::None;
}