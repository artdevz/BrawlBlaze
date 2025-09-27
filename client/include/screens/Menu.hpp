#pragma once

#include "screens/Screen.hpp"
#include "utils/Button.hpp"

class Menu : public Screen {

public:
    Menu();

    void Init() override;
    void Update() override;
    void Draw() override;

    bool ShouldClose() const override;
    ScreenType NextScreen() override;

private:
    bool shouldClose;
    bool showStartScreen;
    bool showEnterScreen;

    u_int8_t selectedButton = 0;
    std::vector<Button> buttons;

};