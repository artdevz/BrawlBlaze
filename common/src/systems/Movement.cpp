#include "systems/Movement.hpp"

#include <cmath>

#include <iostream>

void Movement::Move(EntityManager& entityManager, float deltaTime) {
    for (auto& entity : entityManager.GetEntities<Position, Velocity>()) {
        auto& position = entityManager.GetComponent<Position>(entity.id);
        auto& velocity = entityManager.GetComponent<Velocity>(entity.id);

        if (velocity.dx && velocity.dy) {
            float magnitude = std::sqrt(velocity.dx * velocity.dx + velocity.dy * velocity.dy);
            float speed = velocity.maxSpeed;
            velocity.dx = (velocity.dx / magnitude) * speed;
            velocity.dy = (velocity.dy / magnitude) * speed;
        }

        // std::cout << "EntityID: [" << entity.id << "]: Velocity: dx: " << velocity.dx << " dy: " << velocity.dy << " MoveSpeed: " << velocity.maxSpeed << "\n";

        auto newX = position.x + velocity.dx * deltaTime;
        auto newY = position.y + velocity.dy * deltaTime;

        bool collided = false;
        for (auto& other : entityManager.GetEntities<Collider>()) {
            if (!entityManager.TryGetComponent<Collider>(entity.id)) break;
            if (other.id == entity.id) continue;

            // Projectiles:

            if (auto* type = entityManager.TryGetComponent<Type>(other.id); type && type->type == EntityType::Projectile) continue;

            if (auto* projectile = entityManager.TryGetComponent<Projectile>(entity.id)) {
                if (other.id == projectile->originID) continue;

                // To-Do: Dano em que atingir
                // To-Do: Destruir projétil ao atingir
                // To-Do: Não ter Fogo-Amigo
            }
            
            // Phyics:

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