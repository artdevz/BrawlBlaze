#include "systems/Render.hpp"

#include <iostream>

#include "core/AssetManager.hpp"
#include "components/Sprite.hpp"

void Render::RenderTile(EntityManager& entityManager) {
    for (auto& entity : entityManager.GetEntities<TileTag, Position, Sprite>()) {
        auto& position = entityManager.GetComponent<Position>(entity.id);
        auto& sprite = entityManager.GetComponent<Sprite>(entity.id);

        DrawTexture(AssetManager::Get().GetTexture(sprite.id), position.x, position.y, WHITE);
    }
}

void Render::RenderActor(EntityManager& entityManager, uint32_t localID) {
    for (auto& entity : entityManager.GetEntities<Position, Sprite>()) {
        if (entityManager.TryGetComponent<TileTag>(entity.id)) continue;
        auto& position = entityManager.GetComponent<Position>(entity.id);
        auto& sprite = entityManager.GetComponent<Sprite>(entity.id);

        if (entityManager.TryGetComponent<Projectile>(entity.id)) {
            auto* localTeam = entityManager.TryGetComponent<Team>(localID);
            auto* projectileTeam = entityManager.TryGetComponent<Team>(entity.id);
            if (!localTeam || !projectileTeam) continue;

            Color projectileColor = RED;
            if (localTeam->color == projectileTeam->color) projectileColor = BLUE;
            DrawTexture(AssetManager::Get().GetTexture(sprite.id), position.x-8, position.y-8, projectileColor);
            continue;
        }

        DrawTexture(AssetManager::Get().GetTexture(sprite.id), position.x-8, position.y-8, WHITE);
    }
}

void Render::RenderLifebar(EntityManager& entityManager, uint32_t localID) {
    for (auto& entity : entityManager.GetEntities<Position, Health>()) {
        auto& position = entityManager.GetComponent<Position>(entity.id);
        auto& health = entityManager.GetComponent<Health>(entity.id);

        Color lifeBarColor = RED;
        if (auto* localTeam = entityManager.TryGetComponent<Team>(localID)) {
            if (auto* team = entityManager.TryGetComponent<Team>(entity.id)) {
                if (team->color == localTeam->color) lifeBarColor = BLUE;
            }
        }
        if (localID == entity.id) lifeBarColor = GREEN;
        DrawRectangle(position.x - 10, position.y - 14, 20 * (health.current / health.max), 2, lifeBarColor);

        if (auto* player = entityManager.TryGetComponent<Player>(entity.id)) DrawText(player->nickname, position.x - MeasureText(player->nickname, 6)/2, position.y - 26, 6, lifeBarColor);
    }
}