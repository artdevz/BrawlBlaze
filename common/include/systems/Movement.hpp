#pragma once

#include "EntityManager.hpp"

#include "components/Position.hpp"
#include "components/Velocity.hpp"

class Movement {

public:
    void Move(EntityManager& entityManager, float deltaTime);

};