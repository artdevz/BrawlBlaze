#include "screens/Join.hpp"

#include "raylib.h"
#include <iostream>
void Join::Init() {
    showGameScreen = false;

    ip.clear();
}
void Join::Update() {
    if (IsKeyPressed(KEY_ESCAPE)) shouldClose = true;

    int key = GetCharPressed();
    
    if (IsKeyPressed(KEY_BACKSPACE) && !ip.empty()) { ip.pop_back(); }

    while (key) {
        if ((key >= 32 && key <= 126) && ip.size() < 22) ip.push_back((char)key);
        key = GetCharPressed();
    }

    if (IsKeyPressed(KEY_ENTER)) showGameScreen = true;
}

void Join::Draw() {
    DrawText("IP:Port ->", 10, 10, 20, WHITE);
    DrawText(ip.c_str(), 120, 10, 20, YELLOW);
}

bool Join::ShouldClose() const { return shouldClose; }

ScreenType Join::NextScreen() {
    if (showGameScreen) {
        showGameScreen = false;
        return ScreenType::Game;
    }
    return ScreenType::None;
}

const std::string Join::GetIp() const { return ip; }