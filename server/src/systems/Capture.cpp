#include "systems/Capture.hpp"

#include <cmath>
#include <iostream>

#include "components/TryPick.hpp"

void Capture::CaptureFlag(EntityManager& entityManager) {
    for (auto& entity : entityManager.GetEntities<TryPick>()) {
        auto* playerPosition = entityManager.TryGetComponent<Position>(entity.id);
        if (!playerPosition) continue;

        for (auto& flag : entityManager.GetEntities<FlagTag>()) {
            auto* flagPosition = entityManager.TryGetComponent<Position>(flag.id);
            if (!flagPosition) continue;

            auto* entityTeam = entityManager.TryGetComponent<Team>(entity.id);
            auto* flagTeam = entityManager.TryGetComponent<Team>(flag.id);

            if (!entityTeam || !flagTeam) continue;
            if (entityTeam->color == flagTeam->color) continue;

            if (!InRange(*playerPosition, *flagPosition, 50.0f)) {
                entityManager.RemoveComponent<TryPick>(entity.id);
                continue;
            }

            if (auto* inventory = entityManager.TryGetComponent<Inventory>(entity.id)) {
                inventory->flag = true;
                inventory->flagID = flag.id;
            }

            if (auto* flagTag = entityManager.TryGetComponent<FlagTag>(flag.id)) {
                if (flagTag->captured) continue;
                flagTag->captured = true;
            }
            std::cout << "[Debug] Pegou uma bandeira!\n";
            entityManager.RemoveComponent<TryPick>(entity.id);
        }
    }
}

void Capture::DeliverFlag(EntityManager& entityManager) {
    for (auto& entity : entityManager.GetEntities<Inventory>()) {
        auto* inventory = entityManager.TryGetComponent<Inventory>(entity.id);
        if (!inventory) continue;

        if (!inventory->flag) continue;

        auto* team = entityManager.TryGetComponent<Team>(entity.id);
        if (!team) continue;

         auto* position = entityManager.TryGetComponent<Position>(entity.id);
        if (!position) continue;

        int8_t side = (team->color == TeamColor::Blue)? 1 : -1;
        if (position->x >= ((side == 1)? -256.0f : 128.0f) && position->x <= (-256.0f * side) + 128.0f && position->y >= ((side == 1)? 128.0f : -256.0f) && position->y <= ((side == 1)? 128.0f : -256.0f) + 128.0f) {
            inventory->flag = false;
            for (auto& match : entityManager.GetEntities<Scoreboard>()) {
                if (auto* scoreboard = entityManager.TryGetComponent<Scoreboard>(match.id)) {
                    if (side == 1) {
                        scoreboard->blueScore++;
                        break;
                    }
                    scoreboard->redScore++;
                }
            }
            entityManager.AddComponent(inventory->flagID, RemoveTag());
            std::cout << "Entregou a Bandeira!\n";
        }
    }
}

bool Capture::InRange(Position& a, Position& b, float maxDistance) { return ( sqrt( pow(a.x - b.x, 2) + pow(a.y - b.y, 2)) <= maxDistance ); }