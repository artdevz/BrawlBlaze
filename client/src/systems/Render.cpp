#include "systems/Render.hpp"

#include <iostream>

#include "core/AssetManager.hpp"
#include "components/Sprite.hpp"

void Render::RenderTile(EntityManager& entityManager) {
    for (auto& entity : entityManager.GetEntities<Position, Sprite>()) {
        if (!entityManager.TryGetComponent<TileTag>(entity.id)) continue;
        auto& position = entityManager.GetComponent<Position>(entity.id);
        auto& sprite = entityManager.GetComponent<Sprite>(entity.id);

        DrawTexture(AssetManager::Get().GetTexture(sprite.id), position.x, position.y, WHITE);
    }
}

void Render::RenderActor(EntityManager& entityManager) {
    for (auto& entity : entityManager.GetEntities<Position, Sprite>()) {
        auto& position = entityManager.GetComponent<Position>(entity.id);
        auto& sprite = entityManager.GetComponent<Sprite>(entity.id);

        DrawTexture(AssetManager::Get().GetTexture(sprite.id), position.x, position.y, WHITE);
    }
}