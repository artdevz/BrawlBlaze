#pragma once

#include "EntityManager.hpp"

class Capture {

public:
    void CaptureFlag(EntityManager& entityManager);
    void DeliverFlag(EntityManager& entityManager);

private:
    bool InRange(Position& a, Position& b, float maxDistance);

};