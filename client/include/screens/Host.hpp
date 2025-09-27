#pragma once

#include "screens/Screen.hpp"

class Host : public Screen {

public:
    Host();

    void Init() override;
    void Update() override;
    void Draw() override;

    bool ShouldClose() const override;
    ScreenType NextScreen() override;

private:
    bool shouldClose;

    bool showGameScreen;
    
};
