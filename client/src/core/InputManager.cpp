#include "core/InputManager.hpp"

#include <raylib.h>

namespace InputManager {

    // ===== Movement ===== //

    bool IsMoveUpPressed() { return IsKeyDown(KEY_W); }
    bool IsMoveDownPressed() { return IsKeyDown(KEY_S); }
    bool IsMoveLeftPressed() { return IsKeyDown(KEY_A); }
    bool IsMoveRightPressed() { return IsKeyDown(KEY_D); }

    // ===== Actions ===== //

    bool IsPickUpFlagPressed() { return IsKeyDown(KEY_E); }

    bool IsBasicAttackPressed() { return IsMouseButtonDown(MOUSE_BUTTON_LEFT) || IsKeyDown(KEY_SPACE); }

}