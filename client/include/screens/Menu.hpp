#pragma once

#include "screens/Screen.hpp"

class Menu : public Screen {

public:
    Menu();

    void Init() override;
    void Update() override;
    void Render() override;

    bool ShouldClose() const override;
    ScreenType NextScreen() override;

private:
    bool shouldClose;
    bool showHostScreen;
    bool showJoinScreen;

};