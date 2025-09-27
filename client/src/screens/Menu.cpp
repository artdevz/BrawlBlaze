#include "screens/Menu.hpp"

#include "raylib.h"

Menu::Menu() {}

void Menu::Init() {
    shouldClose = false;
    showStartScreen = false;
    showEnterScreen = false;

    buttons = {
        { {580, 255, 120, 30}, "Start Game", [this]() { showStartScreen = true; } },
        { {580, 300, 120, 30}, "Enter Game", [this]() { showEnterScreen = true; } },
        { {580, 345, 120, 30}, "Settings", [this]() {  } },
        { {580, 390, 120, 30}, "Credits", [this]() {  } },
        { {580, 435, 120, 30}, "Quit Game", [this]() { shouldClose = true; } }
    };
}

void Menu::Update() {
    if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE)) shouldClose = true;
    
    if (IsKeyPressed(KEY_DOWN)) selectedButton = (selectedButton + 1) % buttons.size();
    if (IsKeyPressed(KEY_UP)) selectedButton = (selectedButton + buttons.size() - 1) % buttons.size();
    if (IsKeyPressed(KEY_ENTER)) buttons[selectedButton].onClick();
    
    for (auto& button : buttons) {
        if (CheckCollisionPointRec(GetMousePosition(), button.bounds)) selectedButton = &button - &buttons[0];
        button.Update(GetMousePosition());
    }
}

void Menu::Draw() {
    ClearBackground(BLACK);
    int width = 1280, height = 720;

    DrawText("BrawlBlaze", width / 2 - MeasureText("BrawlBlaze", width/15) / 2, height/24, width/15, WHITE);
    for (auto& button : buttons) button.Draw(GetMousePosition(), selectedButton == buttons.size()? false : &button == &buttons[selectedButton]);
}

bool Menu::ShouldClose() const { return shouldClose; }

ScreenType Menu::NextScreen() {
    if (showStartScreen) {
        showStartScreen = false;
        return ScreenType::Host;
    }
    if (showEnterScreen) {
        showEnterScreen = false;
        return ScreenType::Join;
    }
    return ScreenType::None;
}