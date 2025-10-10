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

            if (!InRange(*playerPosition, *flagPosition, 50.0f)) {
                entityManager.RemoveComponent<TryPick>(entity.id);
                continue;
            }

            if (auto* inventory = entityManager.TryGetComponent<Inventory>(entity.id)) inventory->flag = true;

            if (auto* flagTag = entityManager.TryGetComponent<FlagTag>(flag.id)) {
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

        uint8_t side = (team->color == TeamColor::Blue)? 1 : -1;
        if (position->x >= (-256.0f * side) && position->x <= (-256.0f * side) + 128.0f && position->y >= (256.0f * side) && position->y <= (256.0f * side) + 128.0f) {
            inventory->flag = false;
            std::cout << "Entregou a Bandeira!\n";
        }
    }
}

bool Capture::InRange(Position& a, Position& b, float maxDistance) { return ( sqrt( pow(a.x - b.x, 2) + pow(a.y - b.y, 2)) <= maxDistance ); }