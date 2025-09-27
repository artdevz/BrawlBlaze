#pragma once

#include <functional>
#include <string>

#include <raylib.h>

struct Button {
    Rectangle bounds;
    std::string text;
    std::function<void()> onClick;

    void Update(const Vector2 target) { if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(target, bounds)) if (onClick) onClick(); }

    void Draw(const Vector2 target, bool selected) {
        bool hovered = CheckCollisionPointRec(target, bounds) || selected;
        int fontSize = 20;

        int textWidth = MeasureText(text.c_str(), fontSize);
        float textX = bounds.x + (bounds.width - textWidth) / 2;
        float textY = bounds.y + (bounds.height - fontSize) / 2;
        DrawText(text.c_str(), textX, textY, fontSize, hovered ? WHITE : Color{160, 160, 160, 255});
    }
};