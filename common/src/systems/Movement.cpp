#include "systems/Movement.hpp"

#include <cmath>

#include <iostream>

void Movement::Move(EntityManager& entityManager, float deltaTime) {
    for (auto& entity : entityManager.GetEntities<Position, Velocity>()) {
        auto& position = entityManager.GetComponent<Position>(entity.id);
        auto& velocity = entityManager.GetComponent<Velocity>(entity.id);

        if (velocity.dx && velocity.dy) {
            float magnitude = std::sqrt(velocity.dx * velocity.dx + velocity.dy * velocity.dy);
            float speed = 100.0f;
            velocity.dx = (velocity.dx / magnitude) * speed;
            velocity.dy = (velocity.dy / magnitude) * speed;
        }

        auto newX = position.x + velocity.dx * deltaTime;
        auto newY = position.y + velocity.dy * deltaTime;

        bool collided = false;
        for (auto& other : entityManager.GetEntities<Collider>()) {
            if (!entityManager.TryGetComponent<Collider>(entity.id)) break;
            if (other.id == entity.id) continue;
            auto* otherPosition = entityManager.TryGetComponent<Position>(other.id);
            auto* otherCollider = entityManager.TryGetComponent<Collider>(other.id);

            if (otherCollider->Intersects({newX, newY}, *otherCollider, *otherPosition)) {
                collided = true;
                std::cout << "COLIDIU!\n";
                break;
            }
        }

        if (!collided) {
            position.x = newX;
            position.y = newY;
        }
    }
}