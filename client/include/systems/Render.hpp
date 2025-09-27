#pragma once

#include <raylib.h>

#include "../../../common/include/EntityManager.hpp"
#include "../../../common/include/components/Position.hpp"

class Render {

public:
    void RenderTile(EntityManager& entityManager);
    void RenderActor(EntityManager& entityManager);

};