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
            if (auto* type = entityManager.TryGetComponent<Type>(other.id); type && type->type == EntityType::Projectile) continue;

            auto* otherPosition = entityManager.TryGetComponent<Position>(other.id);
            auto* otherCollider = entityManager.TryGetComponent<Collider>(other.id);

            if (otherCollider->Intersects({newX, newY}, *otherCollider, *otherPosition)) {

                if (auto* projectile = entityManager.TryGetComponent<Projectile>(entity.id)) {
                    if (other.id == projectile->originID) continue;

                    if (auto* health = entityManager.TryGetComponent<Health>(other.id)) {
                        if (entityManager.TryGetComponent<RemoveTag>(entity.id)) continue;
                        if (auto* team = entityManager.TryGetComponent<Team>(projectile->originID)) {
                            if (auto* otherTeam = entityManager.TryGetComponent<Team>(other.id)) {
                                std::cout << "Entity ID: " << entity.id << " Team: " << (int)team->color << " | Entity ID: " << other.id << " Team: " << (int)otherTeam->color << "\n";
                                if (team->color == otherTeam->color) {
                                    std::cout << "Fogo-Amigo! Ignorando Dano.\n";
                                    continue;
                                }
                            }
                        }
                        health->current -= 10.0f;
                        std::cout << "Entity ID: " << other.id << " took 10 damage! Current HP: " << health->current << "\n";
                        if (health->current <= 0.0f) {
                            std::cout << "[Server] Entity ID: " << other.id << " died.\n";
                            entityManager.AddComponent(other.id, RemoveTag());
                        }
                    }
                    entityManager.AddComponent(entity.id, RemoveTag());
                }

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