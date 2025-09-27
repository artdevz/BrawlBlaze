#include "EntityManager.hpp"

#include <algorithm>

Entity EntityManager::CreateEntity() {
    uint32_t id = entities.size();
    entities[id] = Entity(id);
    return entities[id];
}

void EntityManager::DeleteEntity(uint32_t id) {
    auto it = std::find_if(entities.begin(), entities.end(), [id](const std::pair<const uint32_t, Entity>& entity) {
        return entity.first == id;
    });
    if (it != entities.end()) entities.erase(it);
}

void EntityManager::Clear() {
    entities.clear();
    entityComponents.clear();
}