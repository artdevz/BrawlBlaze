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

            auto* otherPosition = entityManager.TryGetComponent<Position>(other.id);
            auto* otherCollider = entityManager.TryGetComponent<Collider>(other.id);

            // std::cout << "Entity ID: " << entity.id << " checking collision with Entity ID: " << other.id << "\n";
            if (otherCollider->Intersects({entityManager.GetComponent<Position>(entity.id).x, entityManager.GetComponent<Position>(entity.id).y}, *otherCollider, *otherPosition)) {
                std::cout << "Entity ID: " << entity.id << " collided with Entity ID: " << other.id << "\n";
                // TODO: Colisão com outras entidades nunca acontece devido MoveSystem, então o Collision de Projéteis tem q ser aqui
                // if (auto* projectile = entityManager.TryGetComponent<Projectile>(entity.id)) {
                //     if (other.id == projectile->originID) {
                //         std::cout << "Colidiu com o OriginID!" << other.id << " " << projectile->originID << " \n";
                //         std::cout << "Ignorando Dano ao OriginID!\n";
                //         continue;
                //     }
                // }
                // std::cout << "Colidiu com algo!\n";
                // std::cout << "Entity ID: " << entity.id << " Origin Team: " << (int)originTeam->color << " | Entity ID: " << other.id << " Other Team: " << (int)otherTeam->color << "\n";
                // if (originTeam->color == otherTeam->color) {
                //     std::cout << "Entity ID: " << entity.id << " Team: " << (int)originTeam->color << " | Entity ID: " << other.id << " Team: " << (int)otherTeam->color << "\n";
                //     std::cout << "Fogo-Amigo! Ignorando Dano.\n";
                //     continue;
                // }
    
                // if (originTeam && otherTeam) std::cout << "Entity ID: " << entity.id << " Team: " << (int)originTeam->color << " | Entity ID: " << other.id << " Team: " << (int)otherTeam->color << "\n";
                if (originTeam->color == otherTeam->color) {
                    // std::cout << "Fogo-Amigo! Ignorando Dano.\n";
                    std::cout << "Entity ID: " << entity.id << " Team: " << (int)originTeam->color << " | Entity ID: " << other.id << " Team: " << (int)otherTeam->color << "\n";
                    continue;
                }
                std::cout << "Entity ID: " << entity.id << " hit Entity ID: " << other.id << "\n";
                auto* health = entityManager.TryGetComponent<Health>(other.id);
                std::cout << "54\n";
                if (!health) continue;
                std::cout << "55\n";
                health->current -= 10.0f;
                if (health->current <= 0.0f) {
                    std::cout << "[Server] Entity ID: " << other.id << " died.\n";
                    entityManager.AddComponent(other.id, RemoveTag()); 
                }
            }
        }
    }
};