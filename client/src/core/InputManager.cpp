#include "core/InputManager.hpp"

#include <raylib.h>

namespace InputManager {

    bool IsMoveUpPressed() { return IsKeyDown(KEY_W); }
    bool IsMoveDownPressed() { return IsKeyDown(KEY_S); }
    bool IsMoveLeftPressed() { return IsKeyDown(KEY_A); }
    bool IsMoveRightPressed() { return IsKeyDown(KEY_D); }

}