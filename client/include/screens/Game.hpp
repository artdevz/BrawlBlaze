#pragma once

#include <memory>

#include "screens/Screen.hpp"

#include "core/NetworkManager.hpp"

#include "network/Client.hpp"

class Game : public Screen {
    
public:
    Game();

    void Init() override;
    void Update() override;
    void Render() override;

    bool ShouldClose() const override;
    ScreenType NextScreen() override;

private:
    bool shouldClose;

    NetworkManager networkManager;

    std::unique_ptr<Client> client;

};