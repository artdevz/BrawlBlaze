#include "screens/Menu.hpp"

#include "raylib.h"

Menu::Menu() {}

void Menu::Init() {
    shouldClose = false;
    showHostScreen = false;
    showJoinScreen = false;
}

void Menu::Update() {
    if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE)) shouldClose = true;
    if (IsKeyPressed(KEY_ENTER)) showHostScreen = true;
}

void Menu::Render() {
    ClearBackground(BLACK);
    DrawText("BrawlBlaze", 1280/2 - MeasureText("BrawlBlaze", 1280/15) / 2, 720/24, 1280/15, WHITE);
}

bool Menu::ShouldClose() const { return shouldClose; }

ScreenType Menu::NextScreen() {
    if (showHostScreen) {
        showHostScreen = false;
        return ScreenType::Host;
    }
    if (showJoinScreen) {
        showJoinScreen = false;
        return ScreenType::Join;
    }
    return ScreenType::None;
}