#pragma once

#include "screens/Screen.hpp"

class Join : public Screen {
    
public:
    Join();

    void Init() override;
    void Update() override;
    void Draw() override;

    bool ShouldClose() const override;
    ScreenType NextScreen() override;

private:
    bool shouldClose;
    bool showGameScreen;

};