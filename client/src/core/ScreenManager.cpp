#include "core/ScreenManager.hpp"

#include "raylib.h"

#include "screens/Menu.hpp"
#include "screens/Game.hpp"
#include "screens/Host.hpp"
#include "screens/Join.hpp"

void ScreenManager::Push(ScreenType type) {
    auto screen = Create(type);
    if (screen) {
        screen->Init();
        screens.push(std::move(screen));
    }
}

void ScreenManager::Pop() { if (!screens.empty()) screens.pop(); }

void ScreenManager::Run() {
    if (screens.empty()) return;

    auto& current = screens.top();

    BeginDrawing();
    ClearBackground(BLACK);
    current->Update();
    current->Draw();
    EndDrawing();

    if (current->ShouldClose()) {
        Pop();
        return;
    }

    auto next = current->NextScreen();
    if (next != ScreenType::None) Push(next);
}

bool ScreenManager::IsEmpty() const { return screens.empty(); }

std::unique_ptr<Screen> ScreenManager::Create(ScreenType type) {
    switch(type) {
        case ScreenType::Menu: return std::make_unique<Menu>();
        case ScreenType::Host: return std::make_unique<Host>();
        case ScreenType::Join: return std::make_unique<Join>();
        case ScreenType::Game: return std::make_unique<Game>();
        case ScreenType::Exit: return nullptr;
        default: return nullptr;
    }
}