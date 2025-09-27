#include <memory>

#include <raylib.h>

#include "core/ScreenManager.hpp"

int main() {
    SetTraceLogLevel(LOG_TRACE);

    InitWindow(1280, 720, "BrawlBlaze");

    ScreenManager screenManager;
    screenManager.Push(ScreenType::Menu);

    while (!WindowShouldClose() && !screenManager.IsEmpty()) screenManager.Run();

    return 0;
}