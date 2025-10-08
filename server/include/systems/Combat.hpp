#pragma once

#include "EntityManager.hpp"

class Combat {

public:
    void HandleProjectiles(EntityManager& entityManager);

private:
    void DamageEntity(EntityManager& entityManager, uint32_t originID, uint32_t targetID);
    void UpdateKDA(EntityManager& entityManager, uint32_t originID, uint32_t targetID);
    void UpdateWallet(EntityManager& entityManager, uint32_t originID, uint32_t targetID, uint16_t silver = 0, uint16_t gold = 0);

};