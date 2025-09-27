#pragma once

#include <string>

#include "screens/Screen.hpp"

class Join : public Screen {
    
public:
    void Init() override;
    void Update() override;
    void Draw() override;

    bool ShouldClose() const override;
    ScreenType NextScreen() override;

    const std::string GetIp() const;

private:
    bool shouldClose;
    bool showGameScreen;

    std::string ip;

};