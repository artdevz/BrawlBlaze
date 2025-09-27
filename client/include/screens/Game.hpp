#pragma once

#include <memory>

#include "screens/Screen.hpp"

#include "core/NetworkManager.hpp"

#include "network/Client.hpp"

#include "systems/Render.hpp"

#include "../../../common/include/EntityManager.hpp"

class Game : public Screen {
    
public:
    Game();

    void Init() override;
    void Update() override;
    void Draw() override;

    bool ShouldClose() const override;
    ScreenType NextScreen() override;

private:
    bool shouldClose;

    uint32_t localPlayerID;

    EntityManager entityManager;
    NetworkManager networkManager;

    Render render;

    std::unique_ptr<Client> client;

};