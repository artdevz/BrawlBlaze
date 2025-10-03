#include "systems/Combat.hpp"

#include <iostream>

void Combat::HandleProjectiles(EntityManager& entityManager) {
    for (auto& entity : entityManager.GetEntities<Position, Projectile>()) {
        if (entityManager.TryGetComponent<RemoveTag>(entity.id)) continue;

        auto* originTeam = entityManager.TryGetComponent<Team>(entity.id);
        if (!originTeam) continue;

        for (auto& other : entityManager.GetEntities<Team>()) {
            if (other.id == entity.id) continue;
            
            auto* otherTeam = entityManager.TryGetComponent<Team>(other.id);
            if (!otherTeam) continue;

            auto* originPosition = entityManager.TryGetComponent<Position>(entity.id);
            auto* otherPosition = entityManager.TryGetComponent<Position>(other.id);
            auto* otherCollider = entityManager.TryGetComponent<Collider>(other.id);

            if (!originPosition || !otherPosition || !otherCollider) continue;

            if (otherCollider->Intersects(*originPosition, *otherCollider, *otherPosition)) {
                if (originTeam->color == otherTeam->color) continue;

                entityManager.AddComponent(entity.id, RemoveTag());
                auto* health = entityManager.TryGetComponent<Health>(other.id);
                if (!health) continue;

                health->current -= 10.0f;
                if (health->current <= 0.0f) {
                    std::cout << "[Server] Entity ID: " << other.id << " died.\n";
                    if (auto* player = entityManager.TryGetComponent<Player>(other.id)) {
                        std::cout << "[Server] Player " << player->nickname << " has been killed!\n";
                        entityManager.AddComponent(other.id, Dead());
                        continue;
                    }
                    entityManager.AddComponent(other.id, RemoveTag()); 
                }
            }
        }
    }
};