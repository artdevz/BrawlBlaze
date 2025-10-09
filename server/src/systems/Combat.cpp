#include "systems/Combat.hpp"

#include <iostream>

void Combat::HandleProjectiles(EntityManager& entityManager) {
    for (auto& entity : entityManager.GetEntities<Position, Projectile>()) {
        if (entityManager.TryGetComponent<RemoveTag>(entity.id)) continue;

        auto* originTeam = entityManager.TryGetComponent<Team>(entity.id);
        if (!originTeam) continue;

        for (auto& other : entityManager.GetEntities<Team>()) {
            if (other.id == entity.id) continue;

            if (entityManager.TryGetComponent<Dead>(other.id)) continue;
            
            auto* otherTeam = entityManager.TryGetComponent<Team>(other.id);
            if (!otherTeam) continue;

            auto* originPosition = entityManager.TryGetComponent<Position>(entity.id);
            auto* otherPosition = entityManager.TryGetComponent<Position>(other.id);
            auto* otherCollider = entityManager.TryGetComponent<Collider>(other.id);

            if (!originPosition || !otherPosition || !otherCollider) continue;

            if (!otherCollider->Intersects(*originPosition, *otherCollider, *otherPosition)) continue;
            if (originTeam->color == otherTeam->color) continue;

            entityManager.AddComponent(entity.id, RemoveTag());
            auto& projectile = entityManager.GetComponent<Projectile>(entity.id);
            DamageEntity(entityManager, projectile.originID, other.id);          
        }
    }
};

void Combat::DamageEntity(EntityManager& entityManager, uint32_t originID, uint32_t targetID) {
    auto* health = entityManager.TryGetComponent<Health>(targetID);
    if (!health) return;

    if (health->TakeDamage(25.0f)) {
        std::cout << "[Server] Entity ID: " << targetID << " died.\n";
        if (auto* player = entityManager.TryGetComponent<Player>(targetID)) {
            std::cout << "[Server] Player " << player->nickname << " has been killed!\n";
            UpdateKDA(entityManager, originID, targetID);
            UpdateWallet(entityManager, originID, targetID, 100, 0);
            entityManager.AddComponent(targetID, Dead());
            return;
        }

        if (auto* type = entityManager.TryGetComponent<Type>(targetID)) {
            if (type->type == EntityType::Tower) {
                std::cout << "[Server] Tower ID: " << targetID << " has been taken!\n";
                auto* targetTeam = entityManager.TryGetComponent<Team>(targetID);
                auto* originTeam = entityManager.TryGetComponent<Team>(originID);
                if (targetTeam && originTeam) {
                    targetTeam->color = originTeam->color;
                    targetTeam->changed = true;
                    health->MaxHeal();
                    UpdateWallet(entityManager, originID, targetID, 0, 50);
                }
                return;
            }
        }
        entityManager.AddComponent(targetID, RemoveTag());
    }
}

void Combat::UpdateKDA(EntityManager& entityManager, uint32_t originID, uint32_t targetID) {
    if (auto* targetKDA = entityManager.TryGetComponent<KDA>(targetID)) targetKDA->AddDeath();
    if (auto* originKDA = entityManager.TryGetComponent<KDA>(originID)) originKDA->AddKill();
}

void Combat::UpdateWallet(EntityManager& entityManager, uint32_t originID, uint32_t targetID, uint16_t silver, uint16_t gold) {
    if (auto* originWallet = entityManager.TryGetComponent<Wallet>(originID)) originWallet->AddSilver(silver);
    if (auto* originWallet = entityManager.TryGetComponent<Wallet>(originID)) originWallet->AddGold(gold);
}