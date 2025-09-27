#pragma once

#include <cstdint>
#include <memory>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <vector>

#include "components/Position.hpp"
#include "components/Tag.hpp"
#include "components/Type.hpp"

struct Entity {
    uint32_t id;

    Entity() : id(-1) {}
    Entity(uint32_t id) : id(id) {}
};

class EntityManager {

public:
    EntityManager() = default;

    Entity CreateEntity();
    void DeleteEntity(uint32_t id);
    void Clear();

    template<typename T>
    void AddComponent(uint32_t id, T component) {
        auto& componentsMap = entityComponents[id];
        componentsMap[typeid(T)] = std::make_unique<T>(component);
    }

    template<typename T>
    T& GetComponent(int id) {
        auto& componentsMap = entityComponents[id];
        return *static_cast<T*>(componentsMap[typeid(T)].get());
    }

    template<typename T>
    T* TryGetComponent(int id) {
        auto itEntity = entityComponents.find(id);
        if (itEntity == entityComponents.end()) return nullptr;

        auto itComp = itEntity->second.find(typeid(T));
        if (itComp == itEntity->second.end()) return nullptr;

        return static_cast<T*>(itComp->second.get());
    }

    template<typename... Components>
    std::vector<Entity> GetEntities() {
        std::vector<Entity> matchingEntities;

        for (auto& entity : entities) {
            bool hasAllComponents = true;

            ((hasAllComponents &= (entityComponents[entity.first].find(typeid(Components)) != entityComponents[entity.first].end())), ...);
            if (hasAllComponents) matchingEntities.push_back(entity.second);
        }
        return matchingEntities;
    }

private:
    std::unordered_map<uint32_t, Entity> entities;
    std::unordered_map<uint32_t, std::unordered_map<std::type_index, std::shared_ptr<void>>> entityComponents;

};