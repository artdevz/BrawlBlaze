#pragma once

#include <memory>

#include "screens/Screen.hpp"

#include "core/NetworkManager.hpp"

#include "network/Client.hpp"

#include "systems/Render.hpp"

#include "../../../common/include/EntityManager.hpp"
#include "../../../common/include/systems/Movement.hpp"

class Game : public Screen {
    
public:
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

    Movement movement;
    Render render;

    std::unique_ptr<Client> client;

    uint64_t matchTime;

};